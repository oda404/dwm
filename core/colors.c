
#include <dwm/colors.h>
#include <stdlib.h>

const char col_black[] = "#000000";
const char col_gray0[] = "#111111";
const char col_gray1[] = "#222222";
const char col_gray2[] = "#444444";
const char col_gray2p5[] = "#888888";
const char col_gray3[] = "#bbbbbb";
const char col_gray4[] = "#eeeeee";
const char col_cyan[] = "#005577";
const char col_purple[] = "#5F0A87";
const char col_purple1[] = "#0D1821";
const char col_purple2[] = "#344966";
const char col_purple3[] = "#7768AE";
const char col_purple4[] = "#0B132B";
const char col_purple5[] = "#1C2541";
const char col_purple6[] = "#3A506B";
const char col_green[] = "#3BB273";
const char col_yellow[] = "#E1BC29";
const char col_red[] = "#E15554";

typedef struct S_CachedClr
{
    Clr *clr;
    // This only works if the colors passed to the clrs_get_scheme are the ones declared above :)
    const char *fg;
    const char *bg;
} CachedClr;

static CachedClr *g_cached_clrs;
static size_t g_cached_clr_count;

Clr *clrs_get_scheme(Drw *drw, const char *fg, const char *bg)
{
    Clr *hit = NULL;
    for (size_t i = 0; i < g_cached_clr_count; ++i)
    {
        CachedClr *cc = &g_cached_clrs[i];
        if (cc->fg == fg && cc->bg == bg)
        {
            hit = cc->clr;
            break;
        }
    }

    if (!hit)
    {
        CachedClr *tmp = realloc(g_cached_clrs, (g_cached_clr_count + 1) * sizeof(CachedClr));
        if (!tmp)
            return NULL;

        g_cached_clrs = tmp;
        ++g_cached_clr_count;

        tmp = &g_cached_clrs[g_cached_clr_count - 1];
        tmp->fg = fg;
        tmp->bg = bg;
        tmp->clr = drw_scm_create(drw, &tmp->fg, 2);
        hit = tmp->clr;
    }

    return hit;
}

void clrs_free_all()
{
    for (size_t i = 0; i < g_cached_clr_count; ++i)
        free(g_cached_clrs[i].clr);

    if (g_cached_clrs)
        free(g_cached_clrs);
}
