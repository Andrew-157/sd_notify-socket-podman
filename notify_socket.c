#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BACKLOG 5
#define SOCKET_PATH "/etc/custom_sd_notify_socket"  // this file should be removed before call to bind and on program exit

int main() {
	int sockfd;
	struct sockaddr_un server_addr;
	char message[sizeof("RELOADING=1\nMONOTONIC_USEC=18446744073709551615")];  // buffer for biggest possible message I guess
	int numbytes;

	if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
		perror("server: socket");
		exit(1);
	}

	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCKET_PATH);

	// If unix socket file exists before call to bind, you will get: "Address already in use"
	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		close(sockfd);
		perror("server: bind");
		exit(1);
	}

	while (1) {
		if ((numbytes = recvfrom(sockfd, message, sizeof(message), 0, NULL, NULL)) == -1) {
			perror("server: recvfrom");
			exit(1);
		}
		message[numbytes] = '\0';
		printf("Received message from client: %s\n", message);
	}

	close(sockfd);
}
