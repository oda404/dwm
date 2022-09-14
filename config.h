/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx = 0; /* border pixel of windows */
static const unsigned int snap = 32;	/* snap pixel */
static const int showbar = 1;			/* 0 means no bar */
static const int topbar = 1;			/* 0 means bottom bar */
static const char *fonts[] = {"Ubuntu Mono Nerd Font:size=15:style=Regular"};
static const char dmenufont[] = "Hack:size=13";
static const char col_black[] = "#000000";
static const char col_gray0[] = "#111111";
static const char col_gray1[] = "#222222";
static const char col_gray2[] = "#444444";
static const char col_gray3[] = "#bbbbbb";
static const char col_gray4[] = "#eeeeee";
static const char col_cyan[] = "#005577";
static const char col_purple[] = "#5F0A87";
static const char col_red[] = "#FF0000";
static const char *colors[][3] = {
	/*               fg         bg          border   */
	[SchemeNorm] = {col_gray3, col_black, col_black},
	[SchemeSel] = {col_red, col_black, col_black},
	//[SchemeSelWindow] = { col_gray4, "#000000", col_purple },
	[SchemeWidget] = {col_gray3, col_black, col_black},
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5"};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{"Gimp", NULL, NULL, 0, 1, -1},
	{"Firefox", NULL, NULL, 1 << 8, 0, -1},
};

static const char *runners[] = {
	/* Wallpaper */
	"feh --bg-scale /home/oda/.local/share/wallpapers/panic1.png",
	/* Tap to click */
	"xinput set-prop \"ELAN469D:00 04F3:304B Touchpad\" $(xinput list-props \"ELAN469D:00 04F3:304B Touchpad\" | grep \"libinput Tapping Enabled (\" | cut -d ')' -f 1 - | cut -d '(' -f 2 -) 1",
	/* Window compositor */
	"picom -b -e 0  --backend glx --glx-no-stencil --glx-no-rebind-pixmap -f -I 0.05 -O 0.08 --opacity-rule \"90:class_g = 'Alacritty'\" --opacity-rule \"75:class_g = 'dwm'\""
};

#include "widgets/time.h"
#include "widgets/speakers.h"
#include "widgets/microphone.h"
#include "widgets/battery.h"
#include "widgets/network.h"
#include "widgets/backlight.h"
#include "widget.h"

#define WIDGET_BATTERY 0
#define WIDGET_TIME 3
#define WIDGET_MICROPHONE 4
#define WIDGET_SPEAKERS 5
#define WIDGET_NETWORK 2
#define WIDGET_BACKLIGHT 1

static Widget widgets[] = {
	[WIDGET_BATTERY] = {
		.init = widget_battery_init,
		.update = widget_battery_update,
		.destroy = widget_battery_destroy,
		.periodic_update = true,
		.update_interval = {.tv_sec = 5, .tv_usec = 0}},
	[WIDGET_BACKLIGHT] = {.icon = "", .init = widget_backlight_init, .update = NULL, .destroy = widget_backlight_destroy, .periodic_update = false},
	[WIDGET_NETWORK] = {.init = widget_network_init, .update = widget_network_update, .destroy = widget_network_destroy, .periodic_update = true, .update_interval = {.tv_sec = 5, .tv_usec = 0}},
	[WIDGET_TIME] = {.icon = NULL, .init = widget_time_init, .update = widget_time_update, .destroy = widget_time_destroy, .periodic_update = true, .update_interval = {.tv_sec = 1, .tv_usec = 0}},
	[WIDGET_MICROPHONE] = {.icon = "", .init = widget_microphone_init, .update = NULL, .destroy = widget_microphone_destroy, .periodic_update = false},
	[WIDGET_SPEAKERS] = { .icon = " ", .init = widget_speakers_init, .update = NULL, .destroy = widget_speakers_destroy, .periodic_update = false}};

/* layout(s) */
static const float mfact = 0.55;	 /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;		 /* number of clients in master area */
static const int resizehints = 1;	 /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{"[T] |", tile}, /* first entry is default */
	{"[F] |", NULL}, /* no layout function means floating behavior */
	{"[M] |", monocle},
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY, TAG)                                          \
	{MODKEY, KEY, view, {.ui = 1 << TAG}},                         \
		{MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}}, \
		{MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},          \
		{MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                           \
	{                                                        \
		.v = (const char *[]) { "/bin/sh", "-c", cmd, NULL } \
	}

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {"dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_black, "-nf", col_gray3, "-sb", col_red, "-sf", col_gray4, NULL};
static const char *termcmd[] = {"alacritty", NULL};

static const void *speakers_plus[] = {"+", &widgets[WIDGET_SPEAKERS], NULL};
static const void *speakers_minus[] = {"-", &widgets[WIDGET_SPEAKERS], NULL};
static const void *speakers_mute[] = {"m", &widgets[WIDGET_SPEAKERS], NULL};

static const void *microphone_plus[] = {"+", &widgets[WIDGET_MICROPHONE], NULL};
static const void *microphone_minus[] = {"-", &widgets[WIDGET_MICROPHONE], NULL};
static const void *microphone_mute[] = {"m", &widgets[WIDGET_MICROPHONE], NULL};

static const void *backlight_raise[] = {"+", &widgets[WIDGET_BACKLIGHT], NULL};
static const void *backlight_lower[] = {"-", &widgets[WIDGET_BACKLIGHT], NULL};

static Key keys[] = {
	/* modifier              key           function                 argument */
	{MODKEY, XK_p, spawn, {.v = dmenucmd}},
	{ControlMask, XK_grave, spawn, {.v = termcmd}},
	{MODKEY, XK_F1, widget_speakers_event, {.v = speakers_mute}},
	{MODKEY, XK_F2, widget_speakers_event, {.v = speakers_minus}},
	{MODKEY, XK_F3, widget_speakers_event, {.v = speakers_plus}},
	{MODKEY, XK_End, widget_microphone_event, {.v = microphone_mute}},
	{MODKEY, XK_minus, widget_microphone_event, {.v = microphone_minus}},
	{MODKEY, XK_equal, widget_microphone_event, {.v = microphone_plus}},
	{MODKEY, XK_Page_Up, widget_backlight_event, {.v = backlight_raise}},
	{MODKEY, XK_Page_Down, widget_backlight_event, {.v = backlight_lower}},
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
	TAGKEYS(XK_1, 0)
		TAGKEYS(XK_2, 1)
			TAGKEYS(XK_3, 2)
				TAGKEYS(XK_4, 3)
					TAGKEYS(XK_5, 4)
						TAGKEYS(XK_6, 5)
							TAGKEYS(XK_7, 6)
								TAGKEYS(XK_8, 7)
									TAGKEYS(XK_9, 8){MODKEY | ShiftMask, XK_q, quit, {0}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ClkLtSymbol, 0, Button1, setlayout, {0}},
	{ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
	{ClkWinTitle, 0, Button2, zoom, {0}},
	{ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
	{ClkClientWin, MODKEY, Button1, movemouse, {0}},
	{ClkClientWin, MODKEY, Button2, togglefloating, {0}},
	{ClkClientWin, MODKEY, Button3, resizemouse, {0}},
	{ClkTagBar, 0, Button1, view, {0}},
	{ClkTagBar, 0, Button3, toggleview, {0}},
	{ClkTagBar, MODKEY, Button1, tag, {0}},
	{ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
