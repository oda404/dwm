
#include <dwm/colors.h>
#include <stdlib.h>

typedef struct S_CachedClr
{
    Clr* clr;
    const char* fg;
    const char* bg;
} CachedClr;

static CachedClr* g_cached_clrs;
static size_t g_cached_clr_count;

Clr* clrs_get_scheme(Drw* drw, const char* fg, const char* bg)
{
    Clr* hit = NULL;
    for (size_t i = 0; i < g_cached_clr_count; ++i)
    {
        CachedClr* cc = &g_cached_clrs[i];
        if (cc->fg == fg && cc->bg == bg)
        {
            hit = cc->clr;
            break;
        }
    }

    if (!hit)
    {
        CachedClr* tmp = realloc(
            g_cached_clrs, (g_cached_clr_count + 1) * sizeof(CachedClr));
        if (!tmp)
            return NULL;

        g_cached_clrs = tmp;
        ++g_cached_clr_count;

        tmp = &g_cached_clrs[g_cached_clr_count - 1];
        tmp->fg = fg;
        tmp->bg = bg;
        const char* arr[] = {tmp->fg, tmp->bg};
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
