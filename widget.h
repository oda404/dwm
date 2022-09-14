
#ifndef DWM_WIDGET_H
#define DWM_WIDGET_H

#include<sys/time.h>
#include<stdbool.h>

struct S_Widget;
typedef bool(*widget_init)(struct S_Widget *w);
typedef bool(*widget_update)(struct S_Widget *w);
typedef void(*widget_destroy)(struct S_Widget *w);

typedef struct S_Widget
{
	char *icon; // Generally an UTF-8 symbol.
	char *text; // The text to be displayed next to the icon, if present.

	bool active;

    widget_init init; // Function to be called when the widget is initialized.
	widget_update update; // The function that is called to update the widget's state.
	widget_destroy destroy; 

	/**
	 * @brief If the widget is to be updated periodically.
	 * If true 'update_interval' timeval needs to be set.
	 * If false the widget is to be updated by other means eg: a keypress.
	 */
	bool periodic_update;
	struct timeval update_interval; // Update interval if the widget is to be updated periodically.
	struct timeval last_update;

	/* If the widget is NOT to be updated periodically, should indicate 
	if the widget has been updated internally and should be redrawn.
	Always false if the widget is to be updated periodically. */
	bool should_redraw;
} Widget;

#endif // !DWM_WIDGET_H
