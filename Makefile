server.out: unix_socket_server.c
	gcc -Wall -o server.out unix_socket_server.c

client.out: unix_socket_client.c
	gcc -Wall -o client.out unix_socket_client.c

clean:
	rm -vf *.out unix_socket
