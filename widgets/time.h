
#ifndef DWM_TIME_WIDGET_H
#define DWM_TIME_WIDGET_H

#include <dwm/widget.h>
#include <stddef.h>

int widget_time_init(struct S_Widget* w);
int widget_time_update(struct S_Widget* w);
void widget_time_destroy(struct S_Widget* w);

#endif // !DWM_TIME_WIDGET_H
