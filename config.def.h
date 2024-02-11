/* See LICENSE file for copyright and license details. */

#include <dwm/colors.h>
#include <dwm/widget.h>
#include <dwm/screenshot.h>

#define BAR_X_PADDING 4
#define BAR_Y_PADDING 4

/* appearance */
static const unsigned int borderpx = 4; /* border pixel of windows */
static const unsigned int snap = 32;	/* snap pixel */
static const int showbar = 1;			/* 0 means no bar */
static const int topbar = 1;			/* 0 means bottom bar */
static const bool scroll_window_name = true;
static const char *fonts[] = {"Ubuntu Mono Nerd Font:size=12:style=Bold"};
static const char dmenufont[] = "Ubuntu Mono Nerd Font:size=14";

static const char *colors[][3] = {
	/*               fg         bg          border   */
	[SchemeNorm] = {col_normal_text, col_tranparent},
	[SchemeSel] = {col_normal_text, col_dark_bg},
	[SchemeTagNormal] = {col_dark_highlight_secondary, col_dark_highlight_primary},
};

/* tagging */
static const char *tags[] = {"1", "2", "3", "4", "5"};

static const Rule rules[] = {
	/* class      instance    title       tags mask     isfloating   monitor */
	// {"Gimp", NULL, NULL, 0, 1, -1},
	// {"Firefox", NULL, NULL, 1 << 8, 0, -1},
};

/* Commands executed when dwm starts */
static const char *runners[] = {
	//"systemctl start mac",
	/* Tap to click */
	//"xinput set-prop \"ELAN469D:00 04F3:304B Touchpad\" $(xinput list-props \"ELAN469D:00 04F3:304B Touchpad\" | grep \"libinput Tapping Enabled (\" | cut -d ')' -f 1 - | cut -d '(' -f 2 -) 1",
	/* Window compositor */
	//"picom -b -e 0  --backend glx --glx-no-stencil --glx-no-rebind-pixmap -f -I 0.1 -O 0.1 --corner-radius 10"
};

/* Bar widgets */
#include "widgets/time.h"
#include "widgets/speakers.h"
#include "widgets/microphone.h"
#include "widgets/battery.h"
#include "widgets/network.h"
#include "widgets/backlight.h"
#include "widgets/cpuinfo/cpuload.h"
#include "widgets/mem.h"

#define WIDGET_BATTERY 1
#define WIDGET_TIME 0
#define WIDGET_MICROPHONE 5
#define WIDGET_SPEAKERS 6
#define WIDGET_NETWORK 4
#define WIDGET_BACKLIGHT 7
#define WIDGET_CPULOAD 2
#define WIDGET_MEM 3

static Widget widgets[] = {
	[WIDGET_BATTERY] = {
		.show_on_monitors = "0",
		.init = widget_battery_init,				   //
		.update = widget_battery_update,			   //
		.destroy = widget_battery_destroy,			   //
		.update_interval = {.tv_sec = 2, .tv_usec = 0} //
	},
	[WIDGET_BACKLIGHT] = {
		.icon = "",
		.show_on_monitors = "0",			//
		.init = widget_backlight_init,		//
		.destroy = widget_backlight_destroy //
	},
	[WIDGET_NETWORK] = {
		.init = widget_network_init,
		.show_on_monitors = "0",					   //
		.update = widget_network_update,			   //
		.destroy = widget_network_destroy,			   //
		.update_interval = {.tv_sec = 1, .tv_usec = 0} //
	},
	[WIDGET_TIME] = {
		.init = widget_time_init,
		.update = widget_time_update,				   //
		.destroy = widget_time_destroy,				   //
		.update_interval = {.tv_sec = 1, .tv_usec = 0} //
	},
	[WIDGET_MICROPHONE] = {
		.icon = "",
		.init = widget_microphone_init,		 //
		.update = NULL,						 //
		.destroy = widget_microphone_destroy //
	},
	[WIDGET_SPEAKERS] = {
		.icon = " ",
		.init = widget_speakers_init,	   //
		.update = NULL,					   //
		.destroy = widget_speakers_destroy //
	},
	[WIDGET_CPULOAD] = {
		.init = widget_cpuload_init,
		.update = widget_cpuload_update,					//
		.destroy = widget_cpuload_destroy,					//
		.update_interval = {.tv_sec = 0, .tv_usec = 100000} //
	},														//
	[WIDGET_MEM] = {
		.init = widget_mem_init,							//
		.update = widget_mem_update,						//
		.destroy = widget_mem_destroy,						//
		.update_interval = {.tv_sec = 0, .tv_usec = 500000} //
	}};

/* layout(s) */
static const float mfact = 0.55;	 /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;		 /* number of clients in master area */
static const int resizehints = 1;	 /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{"T", tile}, /* first entry is default */
	{"F", NULL}, /* no layout function means floating behavior */
	{"M", monocle},
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY, TAG)                                          \
	{MODKEY, KEY, view, {.ui = 1 << TAG}},                         \
		{MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}}, \
		{MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},          \
	{                                                              \
		MODKEY | ControlMask | ShiftMask, KEY, toggletag,          \
		{                                                          \
			.ui = 1 << TAG                                         \
		}                                                          \
	}

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {"dmenu_run", "-i", "-m", dmenumon, "-fn", dmenufont, "-nb", col_black, "-nf", col_gray3, "-sb", col_purple2, "-sf", col_gray4, NULL};
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
	{MODKEY, XK_s, screenshot_start, {0}},
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
