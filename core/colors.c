
#include <dwm/colors.h>
#include <stdlib.h>

const char col_tranparent[] = "#00000000";
const char col_normal_text[] = "#FFbbbbbb";
const char col_dark_text[] = "#FF0D1821";

#define BG_TRANSPARENCY "F0"

const char col_ok[] = "#" BG_TRANSPARENCY "2CA58D";
// const char col_ok[] = "#" BG_TRANSPARENCY "3BB273";
const char col_warn[] = "#" BG_TRANSPARENCY "FBAF00";
// warn F1A208
const char col_danger[] = "#" BG_TRANSPARENCY "D81159";

const char col_dark_bg[] = "#" BG_TRANSPARENCY "0D1821";
const char col_dark_highlight_primary[] = "#" BG_TRANSPARENCY "222E50";
// const char col_dark_highlight_secondary[] = "#" BG_TRANSPARENCY "4F646F";
const char col_dark_highlight_secondary[] = "#" BG_TRANSPARENCY "274690";

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
        const char *arr[] = {tmp->fg, tmp->bg};
        tmp->clr = drw_scm_create(drw, arr, 2);
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
