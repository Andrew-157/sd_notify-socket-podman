/*
 *
 * Exemplary script to be used in systemd service with Type=simple, and WatchdogSec= set, e.g. script should send WATCHDOG=1 with sd_notify
 *
 * */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <systemd/sd-daemon.h>
#include <unistd.h>

#define NOTIFY_SOCKET_ENV "NOTIFY_SOCKET"
#define WATCHDOG_USEC_ENV "WATCHDOG_USEC"
#define USEC_CONVERSION 1000000  // WATCHDOG_USEC is in microseconds
#define FAIL_WATCHDOG_ENV "FAIL_WATCHDOG"  // env to simulate service failure due to too long sleeps between sd_notify calls


void signal_handler(int sig) {
        printf("Caught signal: %d\n", sig);
        closelog();
        exit(0);
}

int main() {
        openlog(NULL, 0, LOG_USER);
        if (getenv(NOTIFY_SOCKET_ENV) == NULL || getenv(WATCHDOG_USEC_ENV) == NULL) {
                syslog(LOG_INFO, "Either '%s' or '%s' environment variables were not set, exiting with failure\n", NOTIFY_SOCKET_ENV, WATCHDOG_USEC_ENV);
                closelog();
                exit(1);
        }

        signal(SIGABRT, signal_handler);
        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);

    syslog(LOG_INFO, "NOTIFY_SOCKET value: %s\n", getenv(NOTIFY_SOCKET_ENV));
        syslog(LOG_INFO, "WATCHDOG_USEC value: %s\n", getenv(WATCHDOG_USEC_ENV));
        syslog(LOG_INFO, "FAIL_WATCHDOG value: %s\n", getenv(FAIL_WATCHDOG_ENV));

        int sleep_time;
        if (getenv(FAIL_WATCHDOG_ENV) != NULL && strcmp(getenv(FAIL_WATCHDOG_ENV), "1") == 0) {
                syslog(LOG_INFO, "Script will be restarted by systemd due to too long sleeps\n");
                sleep_time = atoi(getenv(WATCHDOG_USEC_ENV)) / USEC_CONVERSION + 1;
        } else {
                syslog(LOG_INFO, "Script will keep running until manually stopped or restarted\n");
                sleep_time = atoi(getenv(WATCHDOG_USEC_ENV)) / USEC_CONVERSION;
        }

        syslog(LOG_INFO, "Sleep time value: %d\n", sleep_time);

        while (1) {
                syslog(LOG_INFO, "Script is running...\n");
                sleep(sleep_time);
                // NOTE: accoring to systemd docs, it is recommended to not check return value of the call to sd_notify: https://www.freedesktop.org/software/systemd/man/latest/sd_notify.html#Return%20Value
                sd_notify(0, "WATCHDOG=1");
        }

        closelog();
}
