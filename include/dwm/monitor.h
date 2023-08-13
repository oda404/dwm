
#ifndef DWM_MONITOR_H
#define DWM_MONITOR_H

#include <dwm/layout.h>
#include <dwm/client.h>

typedef struct Monitor Monitor;
struct Monitor
{
    char ltsymbol[16];
    float mfact;
    int nmaster;
    int num;
    int by;             /* bar geometry */
    int mx, my, mw, mh; /* screen size */
    int wx, wy, ww, wh; /* window area  */
    unsigned int seltags;
    unsigned int sellt;
    unsigned int tagset[2];
    int showbar;
    int topbar;
    Client *clients;
    Client *sel;
    Client *stack;
    Monitor *next;
    Window barwin;
    const Layout *lt[2];
    float name_scroll;
};

#endif // !DWM_MONITOR_H
