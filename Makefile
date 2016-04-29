default: client_fork.c server_fork.c client_threads.c server_threads.c client_shmem.c server_shmem.c thr_pool_client.cpp thr_pool_server.cpp
	gcc client_fork.c -o client_fork -g
	gcc server_fork.c -o server_fork -g
	gcc client_threads.c -o client_threads -g
	gcc server_threads.c -lpthread -o server_threads -g
	gcc client_shmem.c -lrt -o client_shmem -g
	gcc server_shmem.c -lrt -o server_shmem -g
	g++ thr_pool_client.cpp -o thr_pool_client -g
	g++ thr_pool_server.cpp -lpthread -o thr_pool_server -g