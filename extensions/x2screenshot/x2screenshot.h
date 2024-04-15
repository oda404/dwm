
#ifndef DWM_EXTENIONS_X2SCREENSHOT_X2SCREENSHOT_H
#define DWM_EXTENIONS_X2SCREENSHOT_X2SCREENSHOT_H

#ifdef DWM_EXTENSIONS_USE_SCREENSHOT
#error                                                                         \
    "DWM_EXTENSIONS_USE_SCREENSHOT has already been defined, only one screenshot extension is supported!"
#endif // DWM_EXTENSIONS_USE_SCREENSHOT

#define DWM_EXTENSIONS_USE_SCREENSHOT 1

#define DWM_EXTENSIONS_SCREENSHOT_CMD ((const char*[]){"x2screenshot", NULL})

#endif // !DWM_EXTENIONS_X2SCREENSHOT_X2SCREENSHOT_H
