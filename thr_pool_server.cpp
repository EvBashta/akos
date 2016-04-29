#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#define THREAD_N 20
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct connection {
	int sock;
	int client_id;
};

std::list<struct connection*> queue;


void *entry(void*) {
  	while (true) {
    	pthread_mutex_lock(&mutex);
    	while (!queue.size()) {
      		assert(!pthread_cond_wait(&cond, &mutex));
    	}
    	struct connection *conn = queue.front();
    	queue.pop_front();
    	pthread_mutex_unlock(&mutex);
    
    	if (!conn->sock) {
      	break;
    	}
    	char filename[256];
		sprintf(filename, "client_%d", conn->client_id);
		FILE *fp = fopen(filename, "w");
		while (1) {
			char c;
			int bytes_read = recv(conn->sock, &c, sizeof(char), 0);
			if (bytes_read <= 0) {
			break;
			}
			fputc(c, fp);
		}
		fclose(fp);
		close(conn->sock);
		free(conn);
  	}
  	return NULL;
}


int main() {
	int listener;
	struct sockaddr_in addr;
	pthread_t *pool = new pthread_t[THREAD_N];
	if(pool == NULL){
		fprintf(stderr, "No memory\n");
		exit(1);
	}
	for (int i = 0; i < THREAD_N; i++) {
  		if(pthread_create(pool + i, NULL, entry, NULL) != 0){
  			perror("pthread_create");
  			exit(2);
  		}
	}

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0) {
		perror("socket");
		exit(3);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(2048);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(4);
	}

	if(listen(listener, 1) == -1) {
		perror("listen");
		exit(5);
	}

	int client_id = 0;

	while (1) {
		struct connection *conn = (struct connection*) malloc(sizeof(struct connection));
		conn->sock = accept(listener, NULL, NULL);
		conn->client_id = client_id;
		++client_id;
		if (conn->sock < 0) {
			perror("socket");
			exit(6);
		}
    	pthread_mutex_lock(&mutex);
    	queue.push_back(conn);
    	pthread_mutex_unlock(&mutex);
    	pthread_cond_signal(&cond);
  	}
  	for (int i = 0; i < THREAD_N; i++) {
    	if(pthread_join(pool[i], NULL) != 0) {
    		perror("pthread_join");
    		exit(7);
    	}
  	}
  	delete[] pool;
  	return 0;
}