// You can also see exemplary unix socket server implementation in man bind
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BACKLOG 5


int main() {
	int sockfd, new_fd;
	struct sockaddr_un server_addr, client_addr;
	socklen_t sin_size;

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("server: socket");
		exit(1);
	}

	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, "unix_socket");

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		close(sockfd);
		perror("server: bind");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		close(sockfd);
		perror("server: listen");
		exit(1);
	}

	while (1) {
		char ch;
		sin_size = sizeof(client_addr);
		new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		read(new_fd, &ch, 1);
		printf("Received from client: %c\n", ch);
		ch++;
		write(new_fd, &ch, 1);
		close(new_fd);
	}

	exit(0);
}
