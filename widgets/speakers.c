
#include "speakers.h"
#include <dwm/audiocon.h>
#include <dwm/colors.h>
#include <stdio.h>

static void on_volume_change(u32 volume, void *userdata)
{
	Widget *w = (Widget *)userdata;

	widget_lock(w);
	snprintf(w->text, WIDGET_TEXT_MAXLEN, "%u%%", volume);
	w->_dirty = true;
	widget_unlock(w);
}

int widget_speakers_init(Widget *w)
{
	if (widget_init_locking(w) < 0)
		return -1;

	if (audiocon_on_output_volume_change(on_volume_change, w) != 0)
		return -1;

	w->active = true;
	w->bgcolor = col_purple5;
	return 0;
}

void widget_speakers_event(const Arg *arg)
{
	if (!arg || !arg->v)
		return;

	const char *cmd = ((const char **)arg->v)[0];
	Widget *w = ((Widget **)arg->v)[1];

	if (!w || !cmd)
		return;

	switch (*cmd)
	{
	case '+':
		audiocon_inc_output_volume(5);
		break;

	case '-':
		audiocon_dec_output_volume(5);
		break;

	case 'm':
		// TODO: mute
		break;

	default:
		break;
	}
}

void widget_speakers_destroy(Widget *w)
{
	widget_destroy_locking(w);
}
