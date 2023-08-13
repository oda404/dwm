
#ifndef DWM_COLORS_H
#define DWM_COLORS_H

#include <dwm/drw.h>

extern const char col_black[];
extern const char col_gray0[];
extern const char col_gray1[];
extern const char col_gray2[];
extern const char col_gray2p5[];
extern const char col_gray3[];
extern const char col_gray4[];
extern const char col_cyan[];
extern const char col_purple[];
extern const char col_purple1[];
extern const char col_purple2[];
extern const char col_purple3[];
extern const char col_purple4[];
extern const char col_purple5[];
extern const char col_purple6[];
extern const char col_green[];
extern const char col_yellow[];
extern const char col_red[];

Clr *clrs_get_scheme(Drw *drw, const char *fg, const char *bg);
void clrs_free_all();

#endif // !DWM_COLORS_H
