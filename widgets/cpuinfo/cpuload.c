
#include "cpuload.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <dwm/colors.h>
#include <stdlib.h>
#include <dwm/types.h>

typedef struct CPUInfo
{
    u64 user;
    u64 nice;
    u64 system;
    u64 idle;
    u64 iowait;
    u64 irq;
    u64 softirq;
    u64 steal;
    u64 guest;
    u64 guest_nice;
} CPUInfo;

static u64 cpuinfo_to_work_time(const CPUInfo *info)
{
    return info->user + info->nice + info->system + info->irq + info->softirq;
}

static u64 cpuinfo_to_total_time(const CPUInfo *info)
{
    return cpuinfo_to_work_time(info) + info->idle + info->iowait + info->guest + info->guest_nice + info->steal;
}

static int cpuline_to_info(char *line, CPUInfo *info)
{
    char *tok = strtok(line, " ");
    for (size_t i = 0; i < 11 && (tok = strtok(NULL, " ")); ++i)
    {
        u64 res = strtoull(tok, NULL, 10);
        ((u64 *)(info))[i] = res;
    }

    return 0;
}

static float rsmin(u64 a, u64 b, u64 def)
{
    if (a > b)
        return (float)(a - b);
    else
        return (float)def;
}

#define PROC_STAT_LINESIZE 256

static CPUInfo *g_cpu_old_states;
static CPUInfo *g_cpu_new_states;
static size_t g_cpu_count;

int widget_cpuload_init(struct S_Widget *w)
{
    g_cpu_count = 0;

    /* Find out how many cores we have */
    FILE *f = fopen("/proc/stat", "r");
    if (!f)
        return 1;

    char line[PROC_STAT_LINESIZE];
    /* Skip first line, it's all the cpus put together? */
    fgets(line, PROC_STAT_LINESIZE, f);

    while (fgets(line, PROC_STAT_LINESIZE, f))
    {
        if (strstr(line, "cpu") != line)
            break;
        ++g_cpu_count;
    }

    fclose(f);

    g_cpu_new_states = calloc(g_cpu_count, sizeof(CPUInfo));
    if (!g_cpu_new_states)
        return 1;

    g_cpu_old_states = calloc(g_cpu_count, sizeof(CPUInfo));
    if (!g_cpu_old_states)
    {
        free(g_cpu_new_states);
        g_cpu_new_states = NULL;
        return 1;
    }

    w->bgcolor = col_dark_highlight_primary;
    widget_cpuload_update(w);
    return 0;
}

bool widget_cpuload_update(struct S_Widget *w)
{
    /* FIXME: maybe switch to mmap? */
    FILE *f = fopen("/proc/stat", "r");
    if (!f)
        return 1;

    size_t written = 0;
    char out[WIDGET_TEXT_MAXLEN];
    out[0] = 0;

    char line[PROC_STAT_LINESIZE];
    /* Skip first line, it's all the cpus put together? */
    fgets(line, PROC_STAT_LINESIZE, f);

    for (size_t cpu = 0; cpu < g_cpu_count; ++cpu)
    {
        fgets(line, PROC_STAT_LINESIZE, f);
        if (strstr(line, "cpu") != line)
        {
            fclose(f);
            return 1; // Something weird happened
        }

        g_cpu_old_states[cpu] = g_cpu_new_states[cpu];
        cpuline_to_info(line, &g_cpu_new_states[cpu]);

        /* This way of calculation core usage has been yoinked from https://github.com/GuillaumeGomez/sysinfo/blob/master/src/unix/linux/cpu.rs */
        u64 total_time_old = cpuinfo_to_total_time(&g_cpu_old_states[cpu]);
        u64 total_time_new = cpuinfo_to_total_time(&g_cpu_new_states[cpu]);

        float cpu_usage =
            rsmin(cpuinfo_to_work_time(&g_cpu_new_states[cpu]), cpuinfo_to_work_time(&g_cpu_old_states[cpu]), 0) /
            rsmin(total_time_new, total_time_old, 1) *
            100.f;

        if (cpu_usage > 100)
            cpu_usage = 100;

        if (written + sizeof("⣿") >= WIDGET_TEXT_MAXLEN)
            break; // dang

        written += sizeof("⣿");

        if (cpu_usage > 87.5)
            strcat(out, "⣿");
        else if (cpu_usage > 75)
            strcat(out, "⣾");
        else if (cpu_usage > 62.5)
            strcat(out, "⣶");
        else if (cpu_usage > 50)
            strcat(out, "⣴");
        else if (cpu_usage > 37.5)
            strcat(out, "⣤");
        else if (cpu_usage > 25)
            strcat(out, "⣠");
        else
            strcat(out, "⣀");
    }

    fclose(f);

    widget_copy_text(w, out);
    return false;
}

void widget_cpuload_destroy(struct S_Widget *w)
{
    free(g_cpu_new_states);
    free(g_cpu_old_states);
    g_cpu_new_states = NULL;
    g_cpu_old_states = NULL;
}
