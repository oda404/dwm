
#ifndef DWM_CLIENT_H
#define DWM_CLIENT_H

typedef struct Monitor Monitor;

typedef struct Client Client;
struct Client
{
    char name[256];
    float mina, maxa;
    int x, y, w, h;
    int oldx, oldy, oldw, oldh;
    int basew, baseh, incw, inch, maxw, maxh, minw, minh, hintsvalid;
    int bw, oldbw;
    unsigned int tags;
    int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen;
    Client *next;
    Client *snext;
    Monitor *mon;
    Window win;
};

#endif // !DWM_CLIENT_H
