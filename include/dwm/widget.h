
#ifndef DWM_WIDGET_H
#define DWM_WIDGET_H

#include <sys/time.h>
#include <stdbool.h>
#include <threads.h>
#include <dwm/types.h>

typedef struct S_Widget
{
	/* The widget icon, generally an UTF-8 symbol. */
	char *icon;

	const char *fgcolor;
	const char *bgcolor;

	/* The body of the widget, displayed to the left of the icon, if any. */
#define WIDGET_TEXT_MAXLEN 72
	char text[WIDGET_TEXT_MAXLEN];

	/* True if the widget should be updated and drawn. */
	bool active;

	int (*init)(struct S_Widget *);
	bool (*update)(struct S_Widget *);
	void (*destroy)(struct S_Widget *);

	struct timeval update_interval; // Update interval if the widget is to be updated periodically.
	struct timeval last_update;

	bool _dirty;

	const char *show_on_monitors;

	/* These two variables are used for widgets which could/do get updated from another thread.
	 * For everyone's safety, everytime you access a widget's public members
	 * (variables without an '_' prefix), you should first call widget_lock(),
	 * and once you're done, widget_unlock(). If the widget is not "multi-threaded", these
	 * functions do nothing. */
	mtx_t _lock;
	bool _should_lock_on_access;
} Widget;

int widget_init(Widget *w);
bool widget_update(const struct timeval *now, Widget *w);

int widget_init_locking(Widget *w);
void widget_destroy_locking(Widget *w);

bool widget_should_be_drawn_on_monitor(Widget *w, u8 mon_num);

void widget_lock(Widget *w);
void widget_unlock(Widget *w);

#endif // !DWM_WIDGET_H
