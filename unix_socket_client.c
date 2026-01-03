#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
	int server_sock;
	struct sockaddr_un server_addr;
	
	if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("client: socket");
		exit(1);
	}

	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, "unix_socket");

	if (connect(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		close(server_sock);
		perror("client: connect");
		exit(1);
	}

	char ch = 'c';
	write(server_sock, &ch, 1);
	read(server_sock, &ch, 1);
	printf("Received: %c\n", ch);

	close(server_sock);
	exit(0);
}
