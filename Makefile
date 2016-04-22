default: client_fork.c server_fork.c client_threads.c server_threads.c client_shmem.c server_shmem.c
	gcc client_fork.c -o client_fork
	gcc server_fork.c -o server_fork
	gcc client_threads.c -o client_threads
	gcc server_threads.c -lpthread -o server_threads
	gcc client_shmem.c -lrt -o client_shmem
	gcc server_shmem.c -lrt -o server_shmem