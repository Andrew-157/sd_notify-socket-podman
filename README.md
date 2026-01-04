# TODO: explain what this repo even is

## How to use sd_notify in C

I am running on fedora, and I had to install systemd-devel:

```bash
dnf install systemd-devel
```

this way you can do

```c
#include <systemd/sd-daemon.h>

int main() {
    sd_notify(0, "READY=1");
}
```

`NOTE:` [`sd_notify` docs](https://www.freedesktop.org/software/systemd/man/latest/sd_notify.html)

when compiling the program, you need to link systemd package

```bash
gcc -Wall watchdog_script.c -lsystemd
```

## sd_notify WATCHDOG=1

`watchdog_script.c` in this repo is the script that could be used in systemd service with [WatchdogSec=](https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html?__goaway_challenge=meta-refresh&__goaway_id=8d1f640f98bf4ab32de4c7f69ef37ce8&__goaway_referer=https%3A%2F%2Fwww.google.com%2F#WatchdogSec=) set, that is, the main process or forked processes (depending on [NotifyAccess=](https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html?__goaway_challenge=meta-refresh&__goaway_id=8d1f640f98bf4ab32de4c7f69ef37ce8&__goaway_referer=https%3A%2F%2Fwww.google.com%2F#NotifyAccess=) set in the service definition) must call `sd_notify` function with `WATCHDOG=1` every <= `WatchdogSec=`, otherwise the process will get `SIGABRT` and the subsequent behavior of systemd is defined by [Restart=](https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html?__goaway_challenge=meta-refresh&__goaway_id=8d1f640f98bf4ab32de4c7f69ef37ce8&__goaway_referer=https%3A%2F%2Fwww.google.com%2F#WatchdogSec=) option in the service definition.

If `FAIL_WATCHDOG` environment variable is not set or set to any other value than "1", `watchdog_script.c` will call `sd_notify` every `WatchdogSec` - 1 seconds, and systemd will not terminate the script with `SIGABRT`, the exemplary service is in srv/watchdog.service.

If `FAIL_WATCHDOG` environment variable is set to "1", `watchdog_script.c` will sleep `WatchdogSec` + 1 seconds between calls to `sd_notify`, which will cause it to get `SIGABRT` from systemd, the exemplary service is in srv/watchdog-fail.service.

