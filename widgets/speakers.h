
#ifndef DWM_SPEAKERS_WIDGET_H
#define DWM_SPEAKERS_WIDGET_H

#include "../widget.h"
#include "../arg.h"
#include <stddef.h>

int widget_speakers_init(struct S_Widget *w);
void widget_speakers_destroy(struct S_Widget *w);
void widget_speakers_event(const Arg *arg);

#endif // !DWM_SPEAKERS_WIDGET_H
