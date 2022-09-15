
#ifndef DWM_BATTERY_WIDGET_H
#define DWM_BATTERY_WIDGET_H

#include "../widget.h"
#include <stddef.h>

int widget_battery_init(struct S_Widget *w);
bool widget_battery_update(struct S_Widget *w);
void widget_battery_destroy(struct S_Widget *w);

#endif // !DWM_BATTERY_WIDGET_H
