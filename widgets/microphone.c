
#include "microphone.h"
#include "../util.h"
#include <stdlib.h>

static const char *g_mic_volume_get_cmd = "awk -F\"[][]\" '/Left:/ { print $2 }' <(amixer sget Capture)";
static const char *g_mic_volume_raise_cmd = "awk -F\"[][]\" '/Left:/ { print $2 }' <(amixer sset Capture 5%+)";
static const char *g_mic_volume_lower_cmd = "awk -F\"[][]\" '/Left:/ { print $2 }' <(amixer sset Capture 5%-)";

#define MICROPHONE_VOLUME_SIZE 5

#define WIDGET_ABORT       \
	{                      \
		w->active = false; \
		return false;      \
	}

bool widget_microphone_init(struct S_Widget *w)
{
	if (!(w->text = malloc(MICROPHONE_VOLUME_SIZE)))
		WIDGET_ABORT;

	if (exec_cmd(g_mic_volume_get_cmd, w->text, MICROPHONE_VOLUME_SIZE) != 0)
		WIDGET_ABORT;

	w->active = true;
	w->should_redraw = true;

	return true;
}

void widget_microphone_destroy(struct S_Widget *w)
{
	if (w->text)
		free(w->text);
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

	if (exec_cmd(amixer_call, w->text, MICROPHONE_VOLUME_SIZE) != 0)
	{
		w->active = false;
		return;
	}

	w->should_redraw = true;
}
