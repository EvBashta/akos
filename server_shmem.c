#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>

#define SHMEM_PATH "/my_shmem"
#define SHMEM_SIZE (1 << 16)
#define SERV_PATH "./serv_path.soc"

int main() {
	int shmem_fd;
	void *ptr;
	int listener, sock;
	struct sockaddr_un addr;
	int bytes = 0;

	if((shmem_fd = shm_open(SHMEM_PATH, O_RDWR | O_CREAT, 0666)) < 0){
		perror("shm_open");
		exit(1);
	}

	if(ftruncate(shmem_fd, SHMEM_SIZE)){
		perror("ftruncate");
		exit(2);
	}

	if(!(ptr = mmap(NULL, SHMEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0))){
		perror("mmap");
		exit(3);
	}

	if ((listener = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("socket");
		exit(4);
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SERV_PATH);
	if (bind (listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(5);
	}

	if (listen(listener, 1)) {
		perror("listen");
		exit(6);
	}

	if ((sock = accept(listener, NULL, NULL)) < 0) {
		perror("accept");
		exit(7);
	}

	if (recv(sock, &bytes, sizeof(int), 0) > 0) {
		int i = 0;
		for (i; i < bytes; ++i)
			printf("%c", *((char*)ptr + i));
	}

	if(unlink(SERV_PATH)) {
		perror("unlink");
		exit(8);
	}

	if(close(sock)){
		perror("close");
		exit(9);
	}

	if(close(listener)){
		perror("close");
		exit(10);
	}

	if(munmap(ptr, SHMEM_SIZE)){
		perror("munmap");
		exit(11);
	}

	if(close(shmem_fd)){
		perror("close");
		exit(12);
	}

	if(shm_unlink(SHMEM_PATH)){
		perror("shm_unlink");
		exit(13);
	}

	return 0;
}