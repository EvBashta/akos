#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

struct connection {
	int sock;
	int client_id;
};

void *thread_func(struct connection *conn) {
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


int main() {
	int listener;
	struct sockaddr_in addr;
	char buf[256];

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0) {
		fprintf(stderr, "Failed to create listener.\n");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(2048);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Failed to bind.\n");
		return 2;
	}

	if(listen(listener, 1) == -1) {
		fprintf(stderr, "Failed to listen.\n");
		return 3;
	}

	int client_id = 0;

	while(1) {
		struct connection *conn = (struct connection*) malloc(sizeof(struct connection));
		conn->sock = accept(listener, NULL, NULL);
		conn->client_id = client_id;
		if (conn->sock < 0) {
			fprintf(stderr, "Failed to accept.\n");
			return 4;
		}
		pthread_t thread;
		thread = pthread_create(&thread, NULL, (void *)thread_func, conn);
		++client_id;
	}
	return 0;
}
