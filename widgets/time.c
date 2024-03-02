
#include "time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static time_t g_lastupdate = 0;

static char* g_weekday_kanjis[] = {"日", "月", "火", "水", "木", "金", "土"};

int widget_time_init(struct S_Widget* w)
{
    widget_time_update(w);
    return 0;
}

int widget_time_update(struct S_Widget* w)
{
    static time_t rawtime;
    static struct tm* timeinfo = NULL;

    rawtime = time(NULL);
    if (rawtime <= g_lastupdate)
        return 0;

    g_lastupdate = rawtime;
    timeinfo = localtime(&rawtime);

    static char clockbuf[9];
    strftime(clockbuf, 9, "%02H:%02M:%02S", timeinfo);

    widget_snprintf_text(
        w,
        "%d月%d日(%s) %s",
        timeinfo->tm_mon + 1,
        timeinfo->tm_mday,
        g_weekday_kanjis[timeinfo->tm_wday],
        clockbuf);

    return 0;
}

void widget_time_destroy(struct S_Widget* w) {}
