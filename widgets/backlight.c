
#include "backlight.h"
#include "../util.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char *g_backlight_get = "xbacklight -get";
static const char *g_backlight_raise =  "xbacklight -inc 5";
static const char *g_backlight_lower = "xbacklight -dec 5";

#define WIDGET_ABORT    \
{                       \
	w->active = false;  \
	return false;       \
}

bool widget_backlight_init(struct S_Widget *w)
{
    if(!(w->text = malloc(5)))
		WIDGET_ABORT;
	
	/* Try to execute the volume_get_cmd to see if it's valid. If not abort. */
	if(!exec_cmd(g_backlight_get, w->text, 4))
		WIDGET_ABORT;

	size_t buflen = strlen(w->text);
	w->text[buflen] = '%';
	w->text[buflen + 1] = '\0';

	w->active = true;
	w->should_redraw = true;
	
	return true;
}

void widget_backlight_event(const Arg *arg)
{
    if(!arg || !arg->v)
		return;
        
	const char *cmd = ((const char **)arg->v)[0];
	Widget *w = ((Widget **)arg->v)[1];

    if(!cmd || !w)
        return;

    const char *xbacklight_call = NULL;

	switch(*cmd)
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

	if(!(exec_cmd(xbacklight_call, NULL, 0) && exec_cmd(g_backlight_get, w->text, 4)))
	{
		w->active = false;
		return;
	}

	size_t buflen = strlen(w->text);
	w->text[buflen] = '%';
	w->text[buflen + 1] = '\0';

	w->should_redraw = true;
}

void widget_backlight_destroy(struct S_Widget *w)
{
    if(w->text)
		free(w->text);
}
