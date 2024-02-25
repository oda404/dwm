
#include <dwm/wallpaper.h>
#include <dwm/util.h>
#include <dwm/cleanup.h>
#include <stdio.h>

#ifdef __linux__
#include <linux/limits.h>
#endif

int wallpaper_set_from_path(const char *wp_path)
{
    _CLEANUP_FREE const char *wp_path_norm = path_normalize_alloc(wp_path);
    if (!wp_path_norm)
        return -1;

    /* including the cmd. I know this is weird but it covers most cases */
    char cmdbuf[PATH_MAX];
    snprintf(cmdbuf, PATH_MAX, "feh --bg-scale %s", wp_path_norm);
    exec_cmd(cmdbuf, NULL, 0);

    return 0;
}
