
#include "microphone.h"
#include "../util.h"
#include <stdlib.h>

static const char *g_mic_volume_get_cmd = "awk -F\"[][]\" '/Left:/ { print $2 }' <(amixer sget Capture)";
static const char *g_mic_volume_raise_cmd = "awk -F\"[][]\" '/Left:/ { print $2 }' <(amixer sset Capture 5%+)";
static const char *g_mic_volume_lower_cmd = "awk -F\"[][]\" '/Left:/ { print $2 }' <(amixer sset Capture 5%-)";

#define WIDGET_ABORT       \
	{                      \
		w->active = false; \
		return 1;          \
	}

int widget_microphone_init(struct S_Widget *w)
{
	if (exec_cmd(g_mic_volume_get_cmd, w->text, WIDGET_TEXT_MAXLEN) != 0)
		WIDGET_ABORT;

	w->active = true;
	w->_dirty = true;

	return 0;
}

void widget_microphone_destroy(struct S_Widget *w)
{
}

void widget_microphone_event(const Arg *arg)
{
	if (!arg || !arg->v)
		return;

	const char *cmd = ((const char **)arg->v)[0];
	Widget *w = ((Widget **)arg->v)[1];

	if (!w || !cmd)
		return;

	const char *amixer_call = NULL;

	switch (*cmd)
	{
	case '+':
		amixer_call = g_mic_volume_raise_cmd;
		break;
	case '-':
		amixer_call = g_mic_volume_lower_cmd;
		break;
	case 'm':
		// FIXME
		amixer_call = "";
		return;
	default:
		return;
	}

	if (exec_cmd(amixer_call, w->text, WIDGET_TEXT_MAXLEN) != 0)
	{
		w->active = false;
		return;
	}

	w->_dirty = true;
}
