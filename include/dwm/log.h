
#ifndef DWM_LOG_H
#define DWM_LOG_H

#include <dwm/types.h>

#define LOG_INFO 1
#define LOG_WARN 2
#define LOG_ERR 3
#define LOG_FATAL 4

int log_init();
int log_print(u8 lvl, const char *fmt, ...);
void log_destroy();

#endif // !DWM_LOG_H
