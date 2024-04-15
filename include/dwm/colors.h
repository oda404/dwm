
#ifndef DWM_COLORS_H
#define DWM_COLORS_H

#include <dwm/drw.h>

Clr* clrs_get_scheme(Drw* drw, const char* fg, const char* bg);
void clrs_free_all();

#endif // !DWM_COLORS_H
