/**
 * Copyright Alexandru Olaru
 * See LICENSE file for copyright and license details
 */

#include <ctype.h>
#include <dirent.h>
#include <ltils/cleanup.h>
#include <ltils/proc/proc.h>
#include <ltils/string/numeric.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/limits.h>
#endif // __linux__

pid_t ltils_proc_find_by_bin_name(const char* name)
{
    LTILS_CLEANUP_CLOSEDIR DIR* d = opendir("/proc");
    if (!d)
        return -1;

    const size_t target_len = strlen(name);
    /* If your compiler doesn't have alloca, womp womp */
    char search_buf[target_len];

    struct dirent* dir;
    while ((dir = readdir(d)) != NULL)
    {
        if (!ltils_string_is_numeric10(dir->d_name))
            continue;

        char comm_path[PATH_MAX];
        snprintf(comm_path, PATH_MAX, "/proc/%s/comm", dir->d_name);

        LTILS_CLEANUP_FCLOSE FILE* f = fopen(comm_path, "r");
        if (!f)
            continue;

        const size_t total_read = fread(search_buf, 1, target_len, f);

        if (total_read < target_len)
            continue;

        if (memcmp(name, search_buf, target_len) == 0)
            return atoi(dir->d_name);
    }

    return -1;
}
