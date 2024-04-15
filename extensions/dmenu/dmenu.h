
#ifndef DWM_EXTENIONS_DMENU_DMENU_H
#define DWM_EXTENIONS_DMENU_DMENU_H

#include <dwm/arg.h>

#ifdef HAVE_EXTENSION_APPMENU
#error                                                                         \
    "HAVE_EXTENSION_APPMENU has already been defined, only one menu extension is supported!"
#endif

#define HAVE_EXTENSION_APPMENU 1

void dwm_extension_appmenu_spawn(const Arg* arg);

#endif // !DWM_EXTENIONS_DMENU_DMENU_H