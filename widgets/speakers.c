
#include "speakers.h"
#include "../util.h"
#include <stdlib.h>

static const char *g_speakers_volume_get_cmd = 
	"awk -v ORS=\"\" -F\"[][]\" '/Left:/ { print $2 }' <(amixer sget Master)";

static const char *g_speakers_volume_raise_cmd = 
	"awk -v ORS=\"\" -F\"[][]\" '/Left:/ { print $2 }' <(amixer sset Master 5%+)";

static const char *g_speakers_volume_lower_cmd = 
	"awk -v ORS=\"\" -F\"[][]\" '/Left:/ { print $2 }' <(amixer sset Master 5%-)";
	
static size_t g_speakers_volume_output_max_size = 5;

#define WIDGET_ABORT    \
{                       \
	w->active = false;  \
	return false;       \
}

bool widget_speakers_init(struct S_Widget *w)
{
	if(!(w->text = malloc(g_speakers_volume_output_max_size)))
		WIDGET_ABORT;
	
	/* Try to execute the volume_get_cmd to see if it's valid. If not abort. */
	if(!exec_cmd(g_speakers_volume_get_cmd, w->text, g_speakers_volume_output_max_size))
		WIDGET_ABORT;

	w->active = true;
	w->should_redraw = true;
	
	return true;
}

void widget_speakers_destroy(struct S_Widget *w)
{
	if(w->text)
		free(w->text);
}

void widget_speakers_event(const Arg *arg)
{
	if(!arg || !arg->v)
		return;
        
	const char *cmd = ((const char **)arg->v)[0];
	Widget *w = ((Widget **)arg->v)[1];

    if(!cmd || !w)
        return;

    const char *amixer_call = NULL;

	switch(*cmd)
	{
	case '+':
		amixer_call = g_speakers_volume_raise_cmd;
		break;
	case '-':
		amixer_call = g_speakers_volume_lower_cmd;
		break;
    case 'm':
        amixer_call = "";
	default:
		return;
	}

	if(!exec_cmd(amixer_call, w->text, g_speakers_volume_output_max_size))
	{
		w->active = false;
		return;
	}
		
	w->should_redraw = true;
}
