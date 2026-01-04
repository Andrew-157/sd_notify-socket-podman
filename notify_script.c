/*
 *
 * Exemplary script to be used in systemd service with Type=notify
 *
 */
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <systemd/sd-daemon.h>
#include <unistd.h>

void signal_handler(int sig) {
	syslog(LOG_INFO, "Caught signal: %d\n", sig);
	closelog();
	exit(0);
}

int main() {
	openlog(NULL, 0, LOG_USER);
	signal(SIGABRT, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	for (int i = 0; i < 5; i++) {
		syslog(LOG_INFO, "Service is starting...\n");
		sleep(1);
	}

	syslog(LOG_INFO, "Service started\n");
	sd_notify(0, "READY=1");

	while (1) {
		syslog(LOG_INFO, "Service is running...\n");
		sleep(3);
	}

	closelog();  // would we ever even get here?
}

