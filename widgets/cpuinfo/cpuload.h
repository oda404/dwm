
#ifndef DWM_CPULOAD_WIDGET_H
#define DWM_CPULOAD_WIDGET_H

#include <dwm/widget.h>
#include <string.h>
#include <stdlib.h>

int widget_cpuload_init(struct S_Widget *w);
bool widget_cpuload_update(struct S_Widget *w);
void widget_cpuload_destroy(struct S_Widget *w);

#endif // !DWM_CPULOAD_WIDGET_H
