
#ifndef DWM_WIDGET_H
#define DWM_WIDGET_H

#include <sys/time.h>
#include <stdbool.h>
#include <threads.h>

typedef struct S_Widget
{
	char *icon; // Generally an UTF-8 symbol.
	char *text; // The text to be displayed next to the icon, if present.

	/* Widget specific data, if any. */
	void *data;
	/* True if the widget should be updated and drawn. */
	bool active;

	bool (*init)(struct S_Widget *);
	bool (*update)(struct S_Widget *);
	void (*destroy)(struct S_Widget *);

	/**
	 * @brief If the widget is to be updated periodically.
	 * If true 'update_interval' timeval needs to be set.
	 * If false the widget is to be updated by other means eg: a keypress.
	 */
	bool periodic_update;
	struct timeval update_interval; // Update interval if the widget is to be updated periodically.
	struct timeval last_update;

	/* These two variables are used for widgets which could/do get updated from another thread.
	 * For everyone's safety, everytime you access a widget's public members
	 * (variables without an '_' prefix), you should first call widget_lock(),
	 * and once you're done, widget_unlock(). If the widget is not "multi-threaded", these
	 * functions do nothing. */
	mtx_t _lock;
	bool _should_lock_on_access;
} Widget;

void widget_lock(Widget *w);
void widget_unlock(Widget *w);

int widget_init(Widget *w);
bool widget_update(const struct timeval *now, Widget *w);

#endif // !DWM_WIDGET_H
