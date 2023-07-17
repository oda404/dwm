#ifndef DWM_BACKLIGHT_WIDGET_H
#define DWM_BACKLIGHT_WIDGET_H

#include <dwm/widget.h>
#include <dwm/arg.h>
#include <stddef.h>

int widget_backlight_init(struct S_Widget *w);
void widget_backlight_event(const Arg *arg);
void widget_backlight_destroy(struct S_Widget *w);

#endif // !DWM_BACKLIGHT_WIDGET_H
