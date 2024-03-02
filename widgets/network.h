
#ifndef DWM_NETWORK_WIDGET_H
#define DWM_NETWORK_WIDGET_H

#include <dwm/widget.h>
#include <stdlib.h>
#include <string.h>

int widget_network_init(struct S_Widget* w);
int widget_network_update(struct S_Widget* w);
void widget_network_destroy(struct S_Widget* w);

#endif // !DWM_NETWORK_WIDGET_H
