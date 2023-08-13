
#ifndef DWM_LAYOUT_H
#define DWM_LAYOUT_H

typedef struct Monitor Monitor;

typedef struct
{
    const char *symbol;
    void (*arrange)(Monitor *);
} Layout;

#endif // !DWM_LAYOUT_H
