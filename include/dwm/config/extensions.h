
#ifndef DWM_CONFIG_EXTENSIONS_H
#define DWM_CONFIG_EXTENSIONS_H

#ifdef HAVE_XNIGHTLIGHT
#include "../../../extensions/xnightlight/xnightlight.h"
#endif // HAVE_XNIGHTLIGHT

#ifdef USE_SLOCK
#include "../../../extensions/slock/slock.h"
#endif // USE_SLOCK

#ifdef USE_ALACRITTY
#include "../../../extensions/alacritty/alacritty.h"
#endif // USE_ALACRITTY

#ifdef USE_DMENU
#include "../../../extensions/dmenu/dmenu.h"
#endif // USE_DMENU

#ifdef USE_X2SCREENSHOT
#include "../../../extensions/x2screenshot/x2screenshot.h"
#endif // USE_X2SCREENSHOT

#endif // !DWM_CONFIG_EXTENSIONS_H
