#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define UNIX_SOCKET "/etc/custom_sd_notify_socket"


static void signal_handler(int sig) {
	printf("Caught signal %d\n", sig);
	exit(0);
}

void remove_socket(void) {
	if (access(UNIX_SOCKET, F_OK) == 0) {
		printf("File %s exists - removing\n", UNIX_SOCKET);
		if (unlink(UNIX_SOCKET) == -1) {
			perror("unlink");
			exit(1);
		}
	} else {
		printf("File %s doesn't exist - nothing to do\n", UNIX_SOCKET);
	}
}

int main() {
	struct sigaction act = {
		.sa_handler = signal_handler,
		.sa_flags = SA_RESTART,
	};

	sigemptyset(&act.sa_mask);
	int signals[2] = {SIGINT, SIGTERM};
	for (int i = 0; i < sizeof(signals) / sizeof(int); i++) {
		int sig = signals[i];
		if (sigaction(sig, &act, NULL) == -1) {
			perror("sigaction");
			exit(1);
		}
	}

	if (atexit(remove_socket) != 0) {
		fprintf(stderr, "Failed to set exit function with atexit\n");
		exit(1);
	}

	int sockfd;
	struct sockaddr_un server_addr;
	char message[sizeof("RELOADING=1\nMONOTONIC_USEC=18446744073709551615")];  // buffer for the biggest possible message I guess
	int numbytes;

	if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
		perror("server: socket");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, UNIX_SOCKET);

	// bind will fail with "Address already in use", if unix socket already exists
	remove_socket();

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
}

