
#include <dwm/util.h>
#include <dwm/widget.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void widget_lock(Widget* w)
{
    if (!w->_should_lock_on_access)
        return;

    mtx_lock(&w->_lock);
}

static void widget_unlock(Widget* w)
{
    if (!w->_should_lock_on_access)
        return;

    mtx_unlock(&w->_lock);
}

static int widget_update(const struct timeval* tv, Widget* w)
{
    widget_lock(w);
    time_t ms_now = timeval_to_ms(tv);
    time_t ms_nextupdate =
        timeval_to_ms(&w->_last_update) + timeval_to_ms(&w->update_interval);

    int ret = 0;
    if (ms_now > ms_nextupdate)
    {
        ret = w->update(w);
        w->_last_update = *tv;
    }
    widget_unlock(w);
    return ret;
}

static void widget_crashed_and_burned(Widget* w)
{
    widget_lock(w);
    w->_active = false;
    widget_unlock(w);
}

static int widget_copy_text_to_backbuffer(Widget* w, const char* text)
{
    char tmpbuf[WIDGET_TEXT_MAXLEN];
    snprintf(
        tmpbuf,
        WIDGET_TEXT_MAXLEN,
        "%s%s%s",
        w->icon ? w->icon : "",
        w->icon ? " " : "",
        text);

    if (strcmp(tmpbuf, w->_text_backbuffer) == 0)
    {
        widget_unlock(w);
        return 0;
    }

    if (strlen(tmpbuf) != strlen(w->_text_backbuffer))
        w->_dirty = WIDGET_DIRTY_LEN_CHANGED;
    else
        w->_dirty = WIDGET_DIRTY_TEXT_CHANGED;

    strncpy(w->_text_backbuffer, tmpbuf, WIDGET_TEXT_MAXLEN - 1);
    return 0;
}

int widget_init(Widget* w)
{
    if (!w->init)
        return 0;

    int st = w->init(w);
    widget_lock(w);
    w->_active = st == 0;
    widget_unlock(w);

    return st;
}

bool widget_active(Widget* w)
{
    bool ret;
    widget_lock(w);
    ret = w->_active;
    widget_unlock(w);
    return ret;
}

int widget_init_locking(Widget* w)
{
    if (mtx_init(&w->_lock, mtx_plain | mtx_recursive) == thrd_error)
        return -1;

    w->_should_lock_on_access = true;
    return 0;
}

void widget_destroy_locking(Widget* w)
{
    mtx_destroy(&w->_lock);
    w->_should_lock_on_access = false;
}

bool widget_should_be_drawn_on_monitor(Widget* w, u8 mon_num)
{
    if (!w->show_on_monitors)
        return true;

    /* Shitty and incomplete implementation but it works for my use case :) */
    unsigned long n = strtoul(w->show_on_monitors, NULL, 10);
    return n == mon_num;
}

int widget_snprintf_text(Widget* w, const char* fmt, ...)
{
    widget_lock(w);

    char text[WIDGET_TEXT_MAXLEN];
    va_list vl;
    va_start(vl, fmt);
    vsnprintf(text, WIDGET_TEXT_MAXLEN, fmt, vl);
    va_end(vl);

    widget_copy_text_to_backbuffer(w, text);
    widget_unlock(w);
    return 0;
}

int widget_copy_text(Widget* w, const char* text)
{
    widget_lock(w);
    widget_copy_text_to_backbuffer(w, text);
    widget_unlock(w);
    return 0;
}

WidgetDirtyType widget_dirty(Widget* w)
{
    widget_lock(w);
    const WidgetDirtyType dirty = w->_dirty;
    widget_unlock(w);
    return dirty;
}

WidgetDirtyType widget_copy_backbuffer(Widget* w, char* outbuf)
{
    WidgetDirtyType ret;

    widget_lock(w);
    ret = w->_dirty;
    strncpy(outbuf, w->_text_backbuffer, WIDGET_TEXT_MAXLEN - 1);
    w->_dirty = WIDGET_DIRTY_CLEAN;
    widget_unlock(w);

    return ret;
}

void widgets_update_periodic(
    Widget* widgets, size_t widget_count, const struct timeval* now)
{
    for (size_t i = 0; i < widget_count; ++i)
    {
        Widget* w = &widgets[i];
        if (!w->update)
            continue;

        if (widget_update(now, w) < 0)
            widget_crashed_and_burned(w);
    }
}

bool widgets_any_dirty(Widget* widgets, size_t widget_count)
{
    for (size_t i = 0; i < widget_count; ++i)
    {
        if (widget_dirty(&widgets[i]) != WIDGET_DIRTY_CLEAN)
            return true;
    }

    return false;
}
