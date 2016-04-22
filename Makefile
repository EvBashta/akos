default: client_fork.c server_fork.c
	gcc client_fork.c -o client_fork
	gcc server_fork.c -o server_fork
