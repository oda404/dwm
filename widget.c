
#include "widget.h"
#include "util.h"

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
    int st = -1;

    if (w->init)
        st = w->init(w);

    if (st != 0)
        w->active = false;

    return st;
}

bool widget_update(const struct timeval *tv, Widget *w)
{
    bool shouldredraw = false;
    time_t ms_now = timeval_to_ms(tv);
    time_t ms_nextupdate = timeval_to_ms(&w->last_update) + timeval_to_ms(&w->update_interval);

    if (ms_now > ms_nextupdate && w->update)
    {
        shouldredraw = w->update(w);
        w->last_update = *tv;
    }

    return shouldredraw;
}
