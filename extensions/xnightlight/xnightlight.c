
#include "xnightlight.h"
#include <ltils/proc/proc.h>
#include <signal.h>
#include <stddef.h>

extern void spawn(const Arg* arg);

void dwm_xnightlight_toggle(const Arg*)
{
    pid_t pid = ltils_proc_find_by_bin_name("xnightlight");
    if (pid > 0)
    {
        kill(pid, SIGKILL);
    }
    else
    {
        const char* lockcmd[] = {"xnightlight", NULL};
        Arg arg1 = {.v = lockcmd};
        spawn(&arg1);
    }
}
