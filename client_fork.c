#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	int s;
	struct sockaddr_in addr;
	char message[256];
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		fprintf(stderr, "Failed to create socket.\n");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(2048);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Connection failed.\n");
		return 2;
	}
	while(1) {
		char c = fgetc(stdin);
		if(c == EOF) {
			break;
		}
		send(s, &c, sizeof(char), 0);
	}
	close(s);
	return 0;
}
