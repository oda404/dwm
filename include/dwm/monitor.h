
#ifndef DWM_MONITOR_H
#define DWM_MONITOR_H

#include <dwm/client.h>
#include <dwm/layout.h>

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
    Client* clients;
    Client* sel;
    Client* stack;
    Monitor* next;
    const Layout* lt[2];

    Window barwin;
    size_t bar_width;
    size_t bar_height;

    /* If the tagset/ltsymbol has changed and needs rerendering */
    bool bar_tags_dirty;

    /* Width of the currently rendered widgets */
    size_t widgets_width;

    float name_scroll;
    /* Tagset animation starting x */
    int start_x;
    /* Tagset animation destination x */
    int end_x;
    /* Tagset animation interpolation t */
    float t;
};

#endif // !DWM_MONITOR_H
