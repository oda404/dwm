
#include "battery.h"
#include <dwm/colors.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct S_Battery
{
    char* capacity_path;
    char* status_path;
} Battery;

typedef enum E_BatteryStatus
{
    BATTERY_CHARGING,
    BATTERY_DISCHARGING,
    BATTERY_FULL
} BatteryStatus;

static Battery* g_battery = NULL;

static char* battery_status_to_icon(const char* status)
{
    if (strncmp(status, "Discharging", 11) == 0)
        return " ";
    else if (strncmp(status, "Charging", 8) == 0)
        return " ";
    else
        return "";
}

int widget_battery_init(struct S_Widget* w)
{
#if defined(__linux__)

#define POWER_SUPPLIES_MAX_N 10
#define POWER_SUPPLY_PATH_LENGTH 64

    char* capacity_path = malloc(POWER_SUPPLY_PATH_LENGTH);
    if (!capacity_path)
        return 1;

    for (size_t i = 0; i < POWER_SUPPLIES_MAX_N; ++i)
    {
        snprintf(
            capacity_path,
            POWER_SUPPLY_PATH_LENGTH,
            "/sys/class/power_supply/BAT%lu/capacity",
            i);

        FILE* file = fopen(capacity_path, "r");
        if (!file)
            continue;

        fclose(file);

        g_battery = malloc(sizeof(Battery));
        if (!g_battery)
            return 1;

        g_battery->capacity_path = malloc(POWER_SUPPLY_PATH_LENGTH);
        g_battery->status_path = malloc(POWER_SUPPLY_PATH_LENGTH);

        if (!(g_battery->capacity_path && g_battery->status_path))
        {
            free(g_battery);
            return 1;
        }

        strncpy(
            g_battery->capacity_path, capacity_path, POWER_SUPPLY_PATH_LENGTH);
        snprintf(
            g_battery->status_path,
            POWER_SUPPLY_PATH_LENGTH,
            "/sys/class/power_supply/BAT%lu/status",
            i);

        break;
    }

    free(capacity_path);

    if (!g_battery)
        return 1;

#endif // defined(__linux__)

    widget_battery_update(w);
    return 0;
}

bool widget_battery_update(struct S_Widget* w)
{
    if (!g_battery)
        return false;

    FILE* fcapacity = fopen(g_battery->capacity_path, "r");
    FILE* fstatus = fopen(g_battery->status_path, "r");
    if (!fcapacity || !fstatus)
        return false;

    char capacity[5];
    fgets(capacity, 5, fcapacity);
    capacity[strlen(capacity) - 1] = '%';

    char status[16];
    fgets(status, 16, fstatus);

    fclose(fcapacity);
    fclose(fstatus);

    widget_snprintf_text(w, "%s%s", capacity, battery_status_to_icon(status));

    unsigned int battery_perc = atoi(capacity);
    if (battery_perc <= 10)
    {
        w->icon = " ";
        w->bgcolor = col_danger;
        w->fgcolor = col_normal_text;
    }
    else if (battery_perc <= 25)
    {
        w->icon = " ";
        w->bgcolor = col_danger;
        w->fgcolor = col_normal_text;
    }
    else if (battery_perc <= 50)
    {
        w->icon = " ";
        w->bgcolor = col_warn;
        w->fgcolor = col_normal_text;
    }
    else if (battery_perc <= 75)
    {
        w->icon = " ";
        w->bgcolor = col_ok;
        w->fgcolor = col_normal_text;
    }
    else
    {
        w->icon = " ";
        w->bgcolor = col_ok;
        w->fgcolor = col_normal_text;
    }

    return true;
}

void widget_battery_destroy(struct S_Widget* w)
{
    if (g_battery->capacity_path)
        free(g_battery->capacity_path);

    if (g_battery->status_path)
        free(g_battery->status_path);
}
