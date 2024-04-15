/* See LICENSE file for copyright and license details. */

#include <dwm/config/colors.h>
#include <dwm/config/extensions.h>
#include <dwm/config/widgets.h>
#include <dwm/widget.h>

/* appearance */
static const unsigned int borderpx = 4; /* border pixel of windows */
static const unsigned int snap = 32;    /* snap pixel */
static const int showbar = 1;           /* 0 means no bar */
static const int topbar = 1;            /* 0 means bottom bar */
static const bool g_scroll_title = false;
static const char* fonts[] = {"Ubuntu Mono Nerd Font:size=10:style=Bold"};

static const char* colors[][2] = {
    /*               fg         bg   */
    [SchemeNorm] = {COL_NORMAL_TEXT, COL_TRANSPARENT},
    [SchemeSel] = {COL_NORMAL_TEXT, COL_DARK_BG},
    [SchemeTagNormal] =
        {COL_DARK_HIGHLIGHT_SECONDARY, COL_DARK_HIGHLIGHT_PRIMARY},
};

/* tagging */
const char* tags[] = {"1", "2", "3", "4", "5"};

static const Rule rules[] = {
    /* class      instance    title       tags mask     isfloating   monitor */
    // {"Gimp", NULL, NULL, 0, 1, -1},
    // {"Firefox", NULL, NULL, 1 << 8, 0, -1},
};

/* layout(s) */
static const float mfact = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen =
    1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"T", tile}, /* first entry is default */
    {"F", NULL}, /* no layout function means floating behavior */
    {"M", monocle},
};

static const char* multimon_setup_path = "~/.dwm.mmsetup";

static const char* wallpaper_path = "~/.local/share/wallpapers/viata.png";

/* Commands executed when dwm starts */
static const char* runners[] = {
    "systemctl --user start mac",
    /* Tap to click */
    "xinput set-prop \"ELAN469D:00 04F3:304B Touchpad\" $(xinput list-props \"ELAN469D:00 04F3:304B Touchpad\" | grep \"libinput Tapping Enabled (\" | cut -d ')' -f 1 - | cut -d '(' -f 2 -) 1",
    /* Window compositor */
    "picom -b"};

enum
{
    WIDGET_TIME = 0,
    WIDGET_BATTERY = 1,
    WIDGET_CPULOAD = 2,
    WIDGET_MEM = 3,
    WIDGET_NETWORK = 4,
    WIDGET_SPEAKERS = 5,
    WIDGET_MICROPHONE = 6,
    WIDGET_BACKLIGHT = 7,
    WIDGET_LAST,
};

Widget widgets[] = {

#ifdef HAVE_WIDGET_BATTERY
    [WIDGET_BATTERY] =
        WIDGET_INSTANCE_PERIODIC(battery, NULL, "0", {.tv_sec = 2}),
#endif

#ifdef HAVE_WIDGET_BACKLIGHT
    [WIDGET_BACKLIGHT] = WIDGET_INSTANCE_VOLATILE(backlight, "", "0"),
#endif

#ifdef HAVE_WIDGET_NETWORK
    [WIDGET_NETWORK] =
        WIDGET_INSTANCE_PERIODIC(network, NULL, "0", {.tv_sec = 1}),
#endif

#ifdef HAVE_WIDGET_TIME
    [WIDGET_TIME] = WIDGET_INSTANCE_PERIODIC(time, NULL, NULL, {.tv_sec = 1}),
#endif

#ifdef HAVE_WIDGET_MICROPHONE
    [WIDGET_MICROPHONE] = WIDGET_INSTANCE_VOLATILE(microphone, "", NULL),
#endif

#ifdef HAVE_WIDGET_SPEAKERS
    [WIDGET_SPEAKERS] = WIDGET_INSTANCE_VOLATILE(speakers, " ", NULL),
#endif

#ifdef HAVE_WIDGET_CPUINFO
    [WIDGET_CPULOAD] =
        WIDGET_INSTANCE_PERIODIC(cpuload, NULL, NULL, {.tv_usec = 100000}),
#endif

#ifdef HAVE_WIDGET_MEM
    [WIDGET_MEM] = WIDGET_INSTANCE_PERIODIC(mem, NULL, NULL, {.tv_sec = 1}),
#endif

    [WIDGET_LAST] = {} //
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY, TAG)                                                      \
    {MODKEY, KEY, view, {.ui = 1 << TAG}},                                     \
        {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},             \
        {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                      \
    {                                                                          \
        MODKEY | ControlMask | ShiftMask, KEY, toggletag,                      \
        {                                                                      \
            .ui = 1 << TAG                                                     \
        }                                                                      \
    }

/* commands */
static Key keys[] = {
/* modifier              key           function                 argument */
#ifdef HAVE_EXTENSION_APPMENU
    {MODKEY, XK_p, dwm_extension_appmenu_spawn, {0}},
#endif

#ifdef HAVE_EXTENSION_LOCK
    {MODKEY | ShiftMask, XK_z, spawn, {.v = DWM_EXTENSIONS_SUSPEND_LOCK_CMD}},

    {MODKEY, XK_z, spawn, {.v = DWM_EXTENSIONS_LOCK_CMD}},
#endif

#ifdef HAVE_EXTENSION_TERM
    {ControlMask, XK_grave, spawn, {.v = DWM_EXTENSIONS_TERM_CMD}},
#endif

#ifdef HAVE_WIDGET_SPEAKERS
    {
        MODKEY,
        XK_F1,
        widget_speakers_event,
        {.v = (const void*[]){"m", &widgets[WIDGET_SPEAKERS], NULL}} //
    },
    {
        MODKEY,
        XK_F2,
        widget_speakers_event,
        {.v = (const void*[]){"-", &widgets[WIDGET_SPEAKERS], NULL}} //
    },
    {
        MODKEY,
        XK_F3,
        widget_speakers_event,
        {.v = (const void*[]){"+", &widgets[WIDGET_SPEAKERS], NULL}} //
    },
#endif

#ifdef HAVE_WIDGET_MICROPHONE
    {
        MODKEY,
        XK_End,
        widget_microphone_event,
        {.v = (const void*[]){"m", &widgets[WIDGET_MICROPHONE], NULL}} //
    },
    {
        MODKEY,
        XK_minus,
        widget_microphone_event,
        {.v = (const void*[]){"-", &widgets[WIDGET_MICROPHONE], NULL}} //
    },
    {
        MODKEY,
        XK_equal,
        widget_microphone_event,
        {.v = (const void*[]){"+", &widgets[WIDGET_MICROPHONE], NULL}} //
    },
#endif

#ifdef HAVE_WIDGET_BACKLIGHT
    {
        MODKEY,
        XK_Page_Up,
        widget_backlight_event,
        {.v = (const void*[]){"+", &widgets[WIDGET_BACKLIGHT], NULL}} //
    },
    {
        MODKEY,
        XK_Page_Down,
        widget_backlight_event,
        {.v = (const void*[]){"-", &widgets[WIDGET_BACKLIGHT], NULL}} //
    },
#endif

#ifdef HAVE_XNIGHTLIGHT
    {MODKEY, XK_n, dwm_extension_xnightlight_toggle, {0}},
#endif

#ifdef DWM_EXTENSIONS_USE_SCREENSHOT
    {MODKEY, XK_s, spawn, {.v = DWM_EXTENSIONS_SCREENSHOT_CMD}},
#endif

    /* vanilla dwm keys */
    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY, XK_j, focusstack, {.i = +1}},
    {MODKEY, XK_k, focusstack, {.i = -1}},
    {MODKEY, XK_i, incnmaster, {.i = +1}},
    {MODKEY, XK_d, incnmaster, {.i = -1}},
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY, XK_Return, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY | ShiftMask, XK_c, killclient, {0}},
    {MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
    {MODKEY, XK_f, setlayout, {.v = &layouts[1]}},
    {MODKEY, XK_m, setlayout, {.v = &layouts[2]}},
    {MODKEY, XK_space, setlayout, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY, XK_comma, focusmon, {.i = -1}},
    {MODKEY, XK_period, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
    TAGKEYS(XK_1, 0),
    TAGKEYS(XK_2, 1),
    TAGKEYS(XK_3, 2),
    TAGKEYS(XK_4, 3),
    TAGKEYS(XK_5, 4),
    TAGKEYS(XK_6, 5),
    TAGKEYS(XK_7, 6),
    TAGKEYS(XK_8, 7),
    TAGKEYS(XK_9, 8),
    {MODKEY | ShiftMask, XK_q, quit, {0}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    // {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
