
#ifndef DWM_COLORS_H
#define DWM_COLORS_H

#include <dwm/drw.h>

extern const char col_dark_bg[];
extern const char col_dark_highlight_secondary[];
extern const char col_dark_highlight_primary[];

extern const char col_ok[];
extern const char col_warn[];
extern const char col_danger[];

extern const char col_tranparent[];
extern const char col_normal_text[];
extern const char col_dark_text[];

Clr *clrs_get_scheme(Drw *drw, const char *fg, const char *bg);
void clrs_free_all();

#endif // !DWM_COLORS_H
