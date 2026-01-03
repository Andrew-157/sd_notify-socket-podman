/*
 * Exemplary script to be used in systemd service with Type=simple, and WatchdogSec= set, e.g. script should send READY=1 with sd_notify
 *
 * */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <systemd/sd-daemon.h>
#include <unistd.h>

#define NOTIFY_SOCKET_ENV "NOTIFY_SOCKET"
#define WATCHDOG_USEC_ENV "WATCHDOG_USEC"

void signal_handler(int sig) {
        printf("Caught signal: %d\n", sig);
        closelog();
        exit(0);
}

int main() {
        openlog(NULL, 0, LOG_USER);

        signal(SIGABRT, signal_handler);
        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);

    syslog(LOG_INFO, "NOTIFY_SOCKET value: %s\n", getenv(NOTIFY_SOCKET_ENV));
        syslog(LOG_INFO, "WATCHDOG_USEC value: %s\n", getenv(WATCHDOG_USEC_ENV));

        while (1) {
                syslog(LOG_INFO, "Script is running...\n");
                sleep(5);
                // NOTE: accoring to systemd docs, it is recommended to not check return value of the call to sd_notify: https://www.freedesktop.org/software/systemd/man/latest/sd_notify.html#Return%20Value
                sd_notify(0, "WATCHDOG=1");
        }

        closelog();
}
