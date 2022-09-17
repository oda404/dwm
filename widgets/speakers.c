
#include "speakers.h"
#include "../audiocon.h"
#include <stdio.h>

static void on_volume_change(u32 volume, void *userdata)
{
	Widget *w = (Widget *)userdata;

	mtx_lock(&w->_lock);
	snprintf(w->text, WIDGET_TEXT_MAXLEN, "%u%%", volume);
	w->_dirty = true;
	mtx_unlock(&w->_lock);
}

int widget_speakers_init(struct S_Widget *w)
{
	w->_should_lock_on_access = true;
	if (mtx_init(&w->_lock, mtx_plain | mtx_recursive) == thrd_error)
		return 1;

	if (audiocon_on_output_volume_change(on_volume_change, w) != 0)
		return 1;

	w->active = true;

	return 0;
}

void widget_speakers_event(const Arg *arg)
{
	if (!arg || !arg->v)
		return;

	const char *cmd = ((const char **)arg->v)[0];
	Widget *w = ((Widget **)arg->v)[1];

	if (!cmd || !w)
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

void widget_speakers_destroy(struct S_Widget *w)
{
	mtx_destroy(&w->_lock);
}
