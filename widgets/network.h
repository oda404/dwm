
#ifndef DWM_NETWORK_WIDGET_H
#define DWM_NETWORK_WIDGET_H

#include "../widget.h"
#include <string.h>
#include <stdlib.h>

bool widget_network_init(struct S_Widget *w);
bool widget_network_update(struct S_Widget *w);
void widget_network_destroy(struct S_Widget *w);

#endif // !DWM_NETWORK_WIDGET_H
