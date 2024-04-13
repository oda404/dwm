/**
 * Copyright Alexandru Olaru
 * See LICENSE file for copyright and license details
 */

#ifndef DWM_WIDGET_H
#define DWM_WIDGET_H

#include <dwm/types.h>
#include <stdbool.h>
#include <sys/time.h>
#include <threads.h>

/* Including the null terminator */
#define WIDGET_TEXT_MAXLEN 128

typedef enum
{
    /* Widget is clean doesn't need rerendering */
    WIDGET_DIRTY_CLEAN,
    /* Widget's contents have changed */
    WIDGET_DIRTY_TEXT_CHANGED,
    /* Widget's contents have changed + their lengths */
    WIDGET_DIRTY_LEN_CHANGED,
} WidgetDirtyType;

typedef struct S_Widget
{
    /* The widget icon, generally an UTF-8 symbol. */
    char* icon;

    const char* fgcolor;
    const char* bgcolor;

    /* State of the text backbuffer */
    WidgetDirtyType _dirty;

    /* The body of the widget, displayed to the left of the icon, if any. */
    char _text_backbuffer[WIDGET_TEXT_MAXLEN];

    /* True if the widget should be updated and drawn. */
    bool _active;

    int (*init)(struct S_Widget*);
    int (*update)(struct S_Widget*);
    void (*destroy)(struct S_Widget*);

    /* If the widget is to be updated periodically (has an update function),
     * this is the interval */
    struct timeval update_interval;
    /* If the widget is to be updated periodically (has an update function(),
     * this is the last time it was updated */
    struct timeval _last_update;

    /* On which monitors to show this widget eg: "0,1,2". NULL means show on all
     */
    const char* show_on_monitors;

    /* These two variables are used for widgets which could/do get updated from
     * another thread. For everyone's safety, everytime you access a widget's
     * public members (variables without an '_' prefix), you should first call
     * widget_lock(), and once you're done, widget_unlock(). If the widget is
     * not "multi-threaded", these functions do nothing. */
    mtx_t _lock;
    bool _should_lock_on_access;
} Widget;

int widget_init(Widget* w);

bool widget_active(Widget* w);

int widget_init_locking(Widget* w);
void widget_destroy_locking(Widget* w);

bool widget_should_be_drawn_on_monitor(Widget* w, u8 mon_num);

int widget_copy_text(Widget* w, const char* text);
int widget_snprintf_text(Widget* w, const char* fmt, ...);

void widgets_update_periodic(
    Widget* widgets, size_t widget_count, const struct timeval* now);

bool widgets_any_dirty(Widget* widgets, size_t widget_count);

WidgetDirtyType widget_copy_backbuffer(Widget* w, char* outbuf);

#endif // !DWM_WIDGET_H
