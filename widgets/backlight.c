
#include "backlight.h"
#include <dwm/util.h>
#include <dwm/colors.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dwm/log.h>

static const char *g_backlight_get = "xbacklight -get";
static const char *g_backlight_raise = "xbacklight -inc 5";
static const char *g_backlight_lower = "xbacklight -dec 5";

int widget_backlight_init(struct S_Widget *w)
{
	/* Try to execute the volume_get_cmd to see if it's valid. If not abort. */
	char backlight[4];
	if (exec_cmd(g_backlight_get, backlight, 4) != 0)
		return -1;

	w->bgcolor = col_purple6;
	widget_snprintf_text(w, "%s%%", backlight);
	return 0;
}

void widget_backlight_event(const Arg *arg)
{
	if (!arg || !arg->v)
		return;

	const char *cmd = ((const char **)arg->v)[0];
	Widget *w = ((Widget **)arg->v)[1];

	if (!cmd || !w)
		return;

	const char *xbacklight_call = NULL;

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
	{
		widget_crashed_and_burned(w);
		return;
	}

	/* My backlight takes a while to update and be accessible again, so we stall a bit */
	i8 tries = 10;
	char backlight[4];
	while (exec_cmd(g_backlight_get, backlight, 4) != 0 && (tries-- > 0))
		usleep(10000);

	if (tries == 0)
	{
		log_print(LOG_WARN, "backlight: Failed to update backlight level after change!");
		return; /* might as well assume the last call also failed :( */
	}

	widget_snprintf_text(w, "%s%%", backlight);
	w->_dirty = true;
}

void widget_backlight_destroy(struct S_Widget *w)
{
}
