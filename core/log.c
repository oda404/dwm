
#include <dwm/log.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/time.h>
#include <dwm/util.h>

#ifndef VERSION
#define VERSION "unknown"
#endif

#define LOG_FAIL           \
    {                      \
        fclose(g_logfile); \
        g_logfile = NULL;  \
        return -1;         \
    }

static FILE *g_logfile;
static double g_start_ms;

int log_init()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    g_start_ms = timeval_to_ms(&tv);

    struct passwd *pw = getpwuid(getuid());

    char logfile_path[PATH_MAX];
    snprintf(logfile_path, PATH_MAX, "%s/.dwm.log", pw->pw_dir);

    g_logfile = fopen(logfile_path, "a");
    if (!g_logfile)
        return -1;

    if (log_print(LOG_INFO, "dwm - %s Starting", VERSION) < 0)
        return -1;

    return 0;
}

int log_print(u8 lvl, const char *fmt, ...)
{
    if (!g_logfile)
        return -1;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    double now_ms = timeval_to_ms(&tv);
    double delta_ms = now_ms - g_start_ms;

    if (fprintf(g_logfile, "%14.5f: ", delta_ms / 1000.0) < 0)
        LOG_FAIL;

    va_list vl;
    va_start(vl, fmt);
    int rc = vfprintf(g_logfile, fmt, vl);
    va_end(vl);

    if (rc < 0)
        LOG_FAIL;

    if (fprintf(g_logfile, "\n") < 0)
        LOG_FAIL;

    if (fflush(g_logfile) < 0)
        LOG_FAIL;

    return 0;
}

void log_destroy()
{
    if (!g_logfile)
        return;

    log_print(LOG_INFO, "dwm - %s Shutting down", VERSION);
    fclose(g_logfile);
    g_logfile = NULL;
}
