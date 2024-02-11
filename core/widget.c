
#include <dwm/widget.h>
#include <dwm/util.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

void widget_lock(Widget *w)
{
    if (!w->_should_lock_on_access)
        return;

    mtx_lock(&w->_lock);
}

void widget_unlock(Widget *w)
{
    if (!w->_should_lock_on_access)
        return;

    mtx_unlock(&w->_lock);
}

int widget_init(Widget *w)
{
    int st = 0;

    if (w->init)
    {
        st = w->init(w);
        w->_active = st == 0;
    }

    return st;
}

int widget_init_locking(Widget *w)
{
    w->_should_lock_on_access = true;
    if (mtx_init(&w->_lock, mtx_plain | mtx_recursive) == thrd_error)
        return -1;

    return 0;
}

void widget_destroy_locking(Widget *w)
{
    mtx_destroy(&w->_lock);
}

bool widget_should_be_drawn_on_monitor(Widget *w, u8 mon_num)
{
    if (!w->show_on_monitors)
        return true;

    /* Shitty and incomplete implementation but it works for my use case :) */
    unsigned long n = strtoul(w->show_on_monitors, NULL, 10);
    return n == mon_num;
}

bool widget_update(const struct timeval *tv, Widget *w)
{
    bool shouldredraw = false;
    time_t ms_now = timeval_to_ms(tv);
    time_t ms_nextupdate = timeval_to_ms(&w->_last_update) + timeval_to_ms(&w->update_interval);

    if (ms_now > ms_nextupdate)
    {
        shouldredraw = w->update(w);
        w->_last_update = *tv;
    }

    return shouldredraw;
}

int widget_snprintf_text(Widget *w, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    int rc = vsnprintf(w->_text, WIDGET_TEXT_MAXLEN, fmt, vl);
    va_end(vl);

    return rc != 0 ? -1 : 0;
}

int widget_copy_text(Widget *w, const char *text)
{
    strncpy(w->_text, text, WIDGET_TEXT_MAXLEN - 1);
    return 0;
}

void widget_crashed_and_burned(Widget *w)
{
    w->_active = false;
}
