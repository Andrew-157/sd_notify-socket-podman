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

