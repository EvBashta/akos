#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	int s, listener, client_id = 0;
	struct sockaddr_in addr;
	//char buf[256];

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
	while(1) {
		s = accept(listener, NULL, NULL);
		if (s < 0) {
			fprintf(stderr, "Failed to accept.\n");
			return 4;
		}
		int a = fork();
		switch(a) {
			case -1:
				fprintf(stderr, "Failed to fork.\n");
				break;
			case 0:
					;
				char filename[256];
				sprintf(filename, "client_%d", client_id);
				FILE* fp = fopen(filename, "w");
				if(close(listener)){
					fprintf(stderr, "Failed to close listenrer\n");
					return 5;
				}
				while(1) {
					char c;
					int bytes_read = recv(s, &c, sizeof(char), 0);
					if (bytes_read <= 0)
						break;
					fputc(c, fp);
				}
				if(close(s)){
					fprintf(stderr, "Failed to close socket\n");
					return 6;
				}
				return 0;
			default:
				++client_id;
				close(s);
		}
	}
	if(close(listener)){
		fprintf(stderr, "Failed to close listenrer\n");
		return 5;
	}
	return 0;
}
