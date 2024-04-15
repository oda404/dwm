#ifndef DWM_MEM_WIDGET_H
#define DWM_MEM_WIDGET_H

#include <dwm/widget.h>
#include <stdlib.h>
#include <string.h>

int widget_mem_init(struct S_Widget* w);
int widget_mem_update(struct S_Widget* w);
void widget_mem_destroy(struct S_Widget* w);

#endif // !DWM_MEM_WIDGET_H
