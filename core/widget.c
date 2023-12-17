
#include <dwm/widget.h>
#include <dwm/util.h>
#include <string.h>
#include <stdlib.h>

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

bool widget_update(const struct timeval *tv, Widget *w)
{
    bool shouldredraw = false;
    time_t ms_now = timeval_to_ms(tv);
    time_t ms_nextupdate = timeval_to_ms(&w->last_update) + timeval_to_ms(&w->update_interval);

    if (ms_now > ms_nextupdate)
    {
        shouldredraw = w->update(w);
        w->last_update = *tv;
    }

    return shouldredraw;
}
