
#ifndef DWM_MICROPHONE_WIDGET_H
#define DWM_MICROPHONE_WIDGET_H

#include "../widget.h"
#include "../arg.h"
#include <stddef.h>

int widget_microphone_init(struct S_Widget *w);
void widget_microphone_destroy(struct S_Widget *w);
void widget_microphone_event(const Arg *arg);

#endif // !DWM_MICROPHONE_WIDGET_H
