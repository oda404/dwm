
#include <dwm/types.h>
#include <dwm/multimon.h>
#include <dwm/cleanup.h>
#include <dwm/util.h>
#include <dwm/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char output_name[32];
    unsigned w;
    unsigned h;
    int x;
    int y;
    char rotation[16];
} MonitorSetup;

#define MMSETUP_LINE_MAX 64

static int multimon_parse_line(const char *line, MonitorSetup *setup)
{
    memset(setup->output_name, 0, 32);
    memset(setup->rotation, 0, 16);

    int st = sscanf(
        line,
        "%31[^,],%u,%u,%d,%d,%8s",
        setup->output_name,
        &setup->w,
        &setup->h,
        &setup->x,
        &setup->y,
        setup->rotation);

    if (st < 6)
        return -1;

    return 0;
}

int multimon_setup_from_path(const char *mmsetup_path)
{
    _CLEANUP_FREE char *mmsetup_path_norm = path_normalize_alloc(mmsetup_path);
    if (!mmsetup_path_norm)
        return -1;

    _CLEANUP_FCLOSE FILE *f = fopen(mmsetup_path_norm, "r");
    if (!f)
    {
        log_print(LOG_WARN, "multimon: Failed to open '%s' for reading!", mmsetup_path);
        return -1;
    }

    char buf[MMSETUP_LINE_MAX];
    while (fgets(buf, MMSETUP_LINE_MAX, f))
    {
        MonitorSetup setup;
        if (multimon_parse_line(buf, &setup) < 0)
        {
            log_print(LOG_ERR, "multimon: Failed to parse line '%s'!", buf);
            return -1;
        }

        char cmdbuf[128];
        snprintf(
            cmdbuf,
            128,
            "xrandr --output %s --mode %ux%u --pos %dx%d --rotate %s",
            setup.output_name,
            setup.w,
            setup.h,
            setup.x,
            setup.y,
            setup.rotation //
        );

        exec_cmd(cmdbuf, NULL, 0); // FIXME: error handling
    }

    return 0;
}
