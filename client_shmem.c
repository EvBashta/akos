#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SHMEM_PATH "/my_shmem"
#define SHMEM_SIZE (1 << 16)
#define SERV_PATH "./serv_path.soc"

int main() {
	int shmem_fd;
	void *ptr;
	int sock;
	struct sockaddr_un addr;
	int bytes = 0;
	char message[256] = "";

	if((shmem_fd = shm_open(SHMEM_PATH, O_RDWR | O_CREAT, 0666)) < 0) {
		perror("shm_open");
		exit(1);
	}

	if(!(ptr = mmap(NULL, SHMEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0))) {
		perror("mmap");
		exit(2);
	}

	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(3);
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SERV_PATH);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("connect");
		exit(4);
	}

	while(bytes < 256) {
		message[bytes] = fgetc(stdin);
		if(feof(stdin))
			break;
		++bytes;
	}

	int i = 0;

	for(i; i < bytes; ++i) {
		*((char*)ptr + i) = message[i];
	}

	send(sock, &bytes, sizeof(int), 0);

	if(close(sock)){
		perror("close");
		exit(5);
	}

	if(munmap(ptr, SHMEM_SIZE)) {
		perror("munmap");
		exit(6);
	}

	if(close(shmem_fd)) {
		perror("close");
		exit(7);
	}

	return 0;
}