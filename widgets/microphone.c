
#include "microphone.h"
#include <dwm/audiocon.h>
#include <dwm/colors.h>
#include <stdio.h>

static void on_volume_change(u32 volume, void *userdata)
{
	Widget *w = (Widget *)userdata;

	widget_lock(w);
	widget_snprintf_text(w, "%u%%", volume);
	w->_dirty = true;
	widget_unlock(w);
}

int widget_microphone_init(Widget *w)
{
	if (widget_init_locking(w) < 0)
		return -1;

	if (audiocon_on_input_volume_change(on_volume_change, w) != 0)
		return -1;

	w->bgcolor = col_purple4;
	return 0;
}

void widget_microphone_event(const Arg *arg)
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
		audiocon_inc_input_volume(5);
		break;

	case '-':
		audiocon_dec_input_volume(5);
		break;

	case 'm':
		// TODO: mute
		break;

	default:
		break;
	}
}

void widget_microphone_destroy(Widget *w)
{
	widget_destroy_locking(w);
}
