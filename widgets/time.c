
#include "time.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TIMEBUF_SIZE 32

static char* g_weekday_kanjis[] = {
	"日",
	"月",
	"火",
	"水",
	"木",
	"金",
	"土"
};

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

	static char clockbuf[9];
	strftime(clockbuf, TIMEBUF_SIZE, "%02H:%02M:%02S", timeinfo);

	snprintf(
		timebuf,
		TIMEBUF_SIZE,
		"%d月%d日(%s) %s",
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		g_weekday_kanjis[timeinfo->tm_wday],
		clockbuf
	);

	strcpy(w->text , timebuf);
	w->should_redraw = true;

	return true;
}

void widget_time_destroy(struct S_Widget *w)
{
	if(w->text)
		free(w->text);
}
