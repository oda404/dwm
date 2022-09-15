
#include "time.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static time_t g_lastupdate = 0;

static char *g_weekday_kanjis[] = {
	"日",
	"月",
	"火",
	"水",
	"木",
	"金",
	"土"};

int widget_time_init(struct S_Widget *w)
{
	w->active = true;
	widget_time_update(w);

	return 0;
}

bool widget_time_update(struct S_Widget *w)
{
	static time_t rawtime;
	static struct tm *timeinfo = NULL;

	rawtime = time(NULL);
	if (rawtime <= g_lastupdate)
		return false;

	g_lastupdate = rawtime;
	timeinfo = localtime(&rawtime);

	static char clockbuf[9];
	strftime(clockbuf, 9, "%02H:%02M:%02S", timeinfo);

	snprintf(
		w->text,
		WIDGET_TEXT_MAXLEN,
		"%d月%d日(%s) %s",
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		g_weekday_kanjis[timeinfo->tm_wday],
		clockbuf);

	w->_dirty = true;

	return true;
}

void widget_time_destroy(struct S_Widget *w)
{
}
