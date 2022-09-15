
#include "backlight.h"
#include "../util.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const char *g_backlight_get = "xbacklight -get";
static const char *g_backlight_raise = "xbacklight -inc 5";
static const char *g_backlight_lower = "xbacklight -dec 5";

#define WIDGET_ABORT       \
	{                      \
		w->active = false; \
		return 1;          \
	}

int widget_backlight_init(struct S_Widget *w)
{
	/* Try to execute the volume_get_cmd to see if it's valid. If not abort. */
	if (exec_cmd(g_backlight_get, w->text, 4) != 0)
		WIDGET_ABORT;

	size_t buflen = strlen(w->text);
	w->text[buflen] = '%';
	w->text[buflen + 1] = '\0';

	w->active = true;
	w->_dirty = true;

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
		w->active = false;
		return;
	}

	/* My backlight takes a while to update and be accessible again, so we stall a bit */
	int tries = 10;
	while (exec_cmd(g_backlight_get, w->text, 4) != 0 && (tries-- > 0))
		usleep(10000);

	if (tries == 0)
		/* might as well assume the last call also failed :( */
		return;

	size_t buflen = strlen(w->text);
	w->text[buflen] = '%';
	w->text[buflen + 1] = '\0';

	w->_dirty = true;
}

void widget_backlight_destroy(struct S_Widget *w)
{
}
