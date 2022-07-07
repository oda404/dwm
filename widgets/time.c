
#include "time.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define TIMEBUF_SIZE 9

bool widget_time_init(struct S_Widget *w)
{
	if(!(w->text = malloc(TIMEBUF_SIZE)))
	{
		w->active = false;
		return false;
	}

	w->active = true;
	widget_time_update(w);

	return true;
}

bool widget_time_update(struct S_Widget *w)
{
	static char timebuf[TIMEBUF_SIZE];

	static time_t rawtime;
	static struct tm *timeinfo = NULL;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(timebuf, TIMEBUF_SIZE, "%02H:%02M:%02S", timeinfo);

	strcpy(w->text , timebuf);
	w->should_redraw = true;

	return true;
}

void widget_time_destroy(struct S_Widget *w)
{
	if(w->text)
		free(w->text);
}
