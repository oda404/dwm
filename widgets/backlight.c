
#include "backlight.h"
#include <ctype.h>
#include <dwm/colors.h>
#include <dwm/log.h>
#include <dwm/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char* g_backlight_get = "xbacklight -get";
static const char* g_backlight_raise = "xbacklight -inc 5";
static const char* g_backlight_lower = "xbacklight -dec 5";

int widget_backlight_init(struct S_Widget* w)
{
    /* Try to execute the volume_get_cmd to see if it's valid. If not abort. */
    char backlight[4];
    if (exec_cmd(g_backlight_get, backlight, 4) != 0)
        return -1;

    widget_snprintf_text(w, "%s%%", backlight);
    return 0;
}

void widget_backlight_event(const Arg* arg)
{
    if (!arg || !arg->v)
        return;

    const char* cmd = ((const char**)arg->v)[0];
    Widget* w = ((Widget**)arg->v)[1];

    if (!cmd || !w)
        return;

    const char* xbacklight_call = NULL;

    switch (*cmd)
    {
    case '+':
        xbacklight_call = g_backlight_raise;
        break;
    case '-':
        xbacklight_call = g_backlight_lower;
        break;
    default:
        return;
    }

    if (exec_cmd(xbacklight_call, NULL, 0) != 0)
        return;

    char backlight[4];
    exec_cmd(g_backlight_get, backlight, 4);
    widget_snprintf_text(w, "%s%%", backlight);
}

void widget_backlight_destroy(struct S_Widget* w) {}
