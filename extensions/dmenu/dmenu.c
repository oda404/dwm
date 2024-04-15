
#include "dmenu.h"
#include <dwm/config/colors.h>
#include <dwm/types.h>

static const char g_dmenufont[] = "Ubuntu Mono Nerd Font:size=13";

extern void spawn(const Arg* arg);

void dwm_extension_appmenu_spawn(const Arg* arg)
{
    const char* dmenucmd[] = {
        "dmenu_run",
        "-m",
        "0", // FIXME: use actual monitor
        "-fn",
        g_dmenufont,
        "-nb",
        COL_DARK_BG,
        "-nf",
        COL_NORMAL_TEXT,
        "-sb",
        COL_DARK_HIGHLIGHT_PRIMARY,
        "-sf",
        "#FFEEEEEE",
        NULL};

    spawn(&((Arg){.v = dmenucmd}));
}
