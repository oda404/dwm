/* See LICENSE file for copyright and license details. */
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dwm/drw.h>
#include <dwm/util.h>

#ifdef DRW_IMG_SUPPORT
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"
#endif // DRW_IMG_SUPPORT

#define LENGTH(X) (sizeof X / sizeof X[0])

#define UTF_INVALID 0xFFFD
#define UTF_SIZ 4

static const unsigned char utfbyte[UTF_SIZ + 1] = {0x80, 0, 0xC0, 0xE0, 0xF0};
static const unsigned char utfmask[UTF_SIZ + 1] = {
    0xC0, 0x80, 0xE0, 0xF0, 0xF8};
static const long utfmin[UTF_SIZ + 1] = {0, 0, 0x80, 0x800, 0x10000};
static const long utfmax[UTF_SIZ + 1] = {
    0x10FFFF, 0x7F, 0x7FF, 0xFFFF, 0x10FFFF};

static long utf8decodebyte(const char c, size_t* i)
{
    for (*i = 0; *i < (UTF_SIZ + 1); ++(*i))
        if (((unsigned char)c & utfmask[*i]) == utfbyte[*i])
            return (unsigned char)c & ~utfmask[*i];
    return 0;
}

static size_t utf8validate(long* u, size_t i)
{
    if (!BETWEEN(*u, utfmin[i], utfmax[i]) || BETWEEN(*u, 0xD800, 0xDFFF))
        *u = UTF_INVALID;
    for (i = 1; *u > utfmax[i]; ++i)
        ;
    return i;
}

static size_t utf8decode(const char* c, long* u, size_t clen)
{
    size_t i, j, len, type;
    long udecoded;

    *u = UTF_INVALID;
    if (!clen)
        return 0;
    udecoded = utf8decodebyte(c[0], &len);
    if (!BETWEEN(len, 1, UTF_SIZ))
        return 1;
    for (i = 1, j = 1; i < clen && j < len; ++i, ++j)
    {
        udecoded = (udecoded << 6) | utf8decodebyte(c[i], &type);
        if (type)
            return j;
    }
    if (j < len)
        return 0;
    *u = udecoded;
    utf8validate(u, len);

    return len;
}

static void xinitvisual(Drw* drw)
{
    XVisualInfo tpl = {.screen = drw->screen, .depth = 32, .class = TrueColor};
    long mask = VisualScreenMask | VisualDepthMask | VisualClassMask;

    int nitems;
    XVisualInfo* infos = XGetVisualInfo(drw->dpy, mask, &tpl, &nitems);
    drw->visual = NULL;

    for (size_t i = 0; i < (size_t)nitems; ++i)
    {
        XRenderPictFormat* fmt =
            XRenderFindVisualFormat(drw->dpy, infos[i].visual);
        if (fmt->type == PictTypeDirect && fmt->direct.alphaMask)
        {
            drw->visual = infos[i].visual;
            drw->depth = infos[i].depth;
            drw->cmap =
                XCreateColormap(drw->dpy, drw->root, drw->visual, AllocNone);
            drw->use_argb = 1;
            break;
        }
    }

    XFree(infos);
    if (!drw->visual)
    {
        drw->visual = DefaultVisual(drw->dpy, drw->screen);
        drw->depth = DefaultDepth(drw->dpy, drw->screen);
        drw->cmap = DefaultColormap(drw->dpy, drw->screen);
    }
}

Drw* drw_create(
    Display* dpy, int screen, Window root, unsigned int w, unsigned int h)
{
    Drw* drw = ecalloc(1, sizeof(Drw));

    drw->dpy = dpy;
    drw->screen = screen;
    drw->root = root;
    drw->w = w;
    drw->h = h;
    xinitvisual(drw);
    drw->drawable = XCreatePixmap(dpy, root, w, h, drw->depth);
    drw->gc = XCreateGC(dpy, drw->drawable, 0, NULL);
    XSetLineAttributes(dpy, drw->gc, 1, LineSolid, CapButt, JoinMiter);

    return drw;
}

void drw_resize(Drw* drw, unsigned int w, unsigned int h)
{
    if (!drw)
        return;

    drw->w = w;
    drw->h = h;
    if (drw->drawable)
        XFreePixmap(drw->dpy, drw->drawable);
    drw->drawable = XCreatePixmap(drw->dpy, drw->root, w, h, drw->depth);
}

void drw_free(Drw* drw)
{
    XFreePixmap(drw->dpy, drw->drawable);
    XFreeGC(drw->dpy, drw->gc);
    drw_fontset_free(drw->fonts);
    free(drw);
}

/* This function is an implementation detail. Library users should use
 * drw_fontset_create instead.
 */
static Fnt* xfont_create(Drw* drw, const char* fontname, FcPattern* fontpattern)
{
    Fnt* font;
    XftFont* xfont = NULL;
    FcPattern* pattern = NULL;

    if (fontname)
    {
        /* Using the pattern found at font->xfont->pattern does not yield the
         * same substitution results as using the pattern returned by
         * FcNameParse; using the latter results in the desired fallback
         * behaviour whereas the former just results in missing-character
         * rectangles being drawn, at least with some fonts. */
        if (!(xfont = XftFontOpenName(drw->dpy, drw->screen, fontname)))
        {
            fprintf(
                stderr, "error, cannot load font from name: '%s'\n", fontname);
            return NULL;
        }
        if (!(pattern = FcNameParse((FcChar8*)fontname)))
        {
            fprintf(
                stderr,
                "error, cannot parse font name to pattern: '%s'\n",
                fontname);
            XftFontClose(drw->dpy, xfont);
            return NULL;
        }
    }
    else if (fontpattern)
    {
        if (!(xfont = XftFontOpenPattern(drw->dpy, fontpattern)))
        {
            fprintf(stderr, "error, cannot load font from pattern.\n");
            return NULL;
        }
    }
    else
    {
        die("no font specified.");
    }

    /* Do not allow using color fonts. This is a workaround for a BadLength
     * error from Xft with color glyphs. Modelled on the Xterm workaround. See
     * https://bugzilla.redhat.com/show_bug.cgi?id=1498269
     * https://lists.suckless.org/dev/1701/30932.html
     * https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=916349
     * and lots more all over the internet.
     */
    FcBool iscol;
    if (FcPatternGetBool(xfont->pattern, FC_COLOR, 0, &iscol) ==
            FcResultMatch &&
        iscol)
    {
        XftFontClose(drw->dpy, xfont);
        return NULL;
    }

    font = ecalloc(1, sizeof(Fnt));
    font->xfont = xfont;
    font->pattern = pattern;
    font->h = xfont->ascent + xfont->descent;
    font->dpy = drw->dpy;

    return font;
}

static void xfont_free(Fnt* font)
{
    if (!font)
        return;
    if (font->pattern)
        FcPatternDestroy(font->pattern);
    XftFontClose(font->dpy, font->xfont);
    free(font);
}

Fnt* drw_fontset_create(Drw* drw, const char* fonts[], size_t fontcount)
{
    Fnt *cur, *ret = NULL;
    size_t i;

    if (!drw || !fonts)
        return NULL;

    for (i = 1; i <= fontcount; i++)
    {
        if ((cur = xfont_create(drw, fonts[fontcount - i], NULL)))
        {
            cur->next = ret;
            ret = cur;
        }
    }
    return (drw->fonts = ret);
}

void drw_fontset_free(Fnt* font)
{
    if (font)
    {
        drw_fontset_free(font->next);
        xfont_free(font);
    }
}

void drw_clr_create(Drw* drw, Clr* dest, const char* clrname)
{
    if (!drw || !dest || !clrname)
        return;

    char rgb[8];
    u32 alpha;

    size_t clrname_len = strlen(clrname);
    if (clrname_len == 9)
    {
        rgb[0] = '#';
        rgb[1] = clrname[3];
        rgb[2] = clrname[4];
        rgb[3] = clrname[5];
        rgb[4] = clrname[6];
        rgb[5] = clrname[7];
        rgb[6] = clrname[8];
        rgb[7] = '\0';

        char stralpha[] = {clrname[1], clrname[2], '\0'};
        alpha = strtoul(stralpha, NULL, 16);
    }
    else if (clrname_len == 7)
    {
        strcpy(rgb, clrname);
        alpha = 0xFF;
    }
    else
    {
        die("Bad color '%s'!", clrname);
    }

    if (!XftColorAllocName(drw->dpy, drw->visual, drw->cmap, rgb, dest))
        die("error, cannot allocate color '%s'", clrname);

    dest->pixel = (dest->pixel & 0x00ffffffU) | (alpha << 24);
}

/* Wrapper to create color schemes. The caller has to call free(3) on the
 * returned color scheme when done using it. */
Clr* drw_scm_create(Drw* drw, const char* clrnames[], size_t clrcount)
{
    size_t i;
    Clr* ret;

    /* need at least two colors for a scheme */
    if (!drw || !clrnames || clrcount < 2 ||
        !(ret = ecalloc(clrcount, sizeof(XftColor))))
        return NULL;

    for (i = 0; i < clrcount; i++)
        drw_clr_create(drw, &ret[i], clrnames[i]);
    return ret;
}

void drw_setfontset(Drw* drw, Fnt* set)
{
    if (drw)
        drw->fonts = set;
}

void drw_setscheme(Drw* drw, Clr* scm)
{
    if (drw)
        drw->scheme = scm;
}

void drw_rect(
    Drw* drw,
    int x,
    int y,
    unsigned int w,
    unsigned int h,
    int filled,
    int invert)
{
    if (!drw || !drw->scheme)
        return;
    XSetForeground(
        drw->dpy,
        drw->gc,
        invert ? drw->scheme[ColBg].pixel : drw->scheme[ColFg].pixel);
    if (filled)
        XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
    else
        XDrawRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w - 1, h - 1);
}

void drw_circle_bordered(
    Drw* drw,
    int x,
    int y,
    unsigned int w,
    unsigned int h,
    int filled,
    int invert)
{
    XSetForeground(
        drw->dpy, drw->gc, drw->scheme[invert ? ColBg : ColFg].pixel);

    XFillRectangle(drw->dpy, drw->drawable, drw->gc, x + h / 2, y, w - h, h);

    XFillArc(drw->dpy, drw->drawable, drw->gc, x, y, h, h, 360, 360 * 64);
    XFillArc(
        drw->dpy,
        drw->drawable,
        drw->gc,
        x + h / 2 + w - h - h / 2,
        y,
        h,
        h,
        360,
        360 * 64);
}

void drw_circle(
    Drw* drw, int x, int y, unsigned int rad, int filled, int invert)
{
    XSetForeground(
        drw->dpy, drw->gc, drw->scheme[invert ? ColBg : ColFg].pixel);

    XGCValues values;
    values.line_width = 2;
    XChangeGC(drw->dpy, drw->gc, GCLineWidth, &values);

    rad -= values.line_width / 2;

    if (filled)
        XFillArc(drw->dpy, drw->drawable, drw->gc, x, y, rad, rad, 0, 360 * 64);
    else
        XDrawArc(drw->dpy, drw->drawable, drw->gc, x, y, rad, rad, 0, 360 * 64);

    values.line_width = 1;
    XChangeGC(drw->dpy, drw->gc, GCLineWidth, &values);
}

int drw_text(
    Drw* drw,
    int x,
    int y,
    unsigned int w,
    unsigned int h,
    unsigned int lpad,
    const char* text,
    int invert)
{
    int i, ty, ellipsis_x = 0;
    unsigned int tmpw, ew, ellipsis_w = 0, ellipsis_len;
    XftDraw* d = NULL;
    Fnt *usedfont, *curfont, *nextfont;
    int utf8strlen, utf8charlen, render = x || y || w || h;
    long utf8codepoint = 0;
    const char* utf8str;
    FcCharSet* fccharset;
    FcPattern* fcpattern;
    FcPattern* match;
    XftResult result;
    int charexists = 0, overflow = 0;
    /* keep track of a couple codepoints for which we have no match. */
    enum
    {
        nomatches_len = 64
    };
    static struct
    {
        long codepoint[nomatches_len];
        unsigned int idx;
    } nomatches;
    static unsigned int ellipsis_width = 0;

    if (!drw || (render && (!drw->scheme || !w)) || !text || !drw->fonts)
        return 0;

    if (!render)
    {
        w = invert ? invert : ~invert;
    }
    else
    {
        drw_circle_bordered(drw, x, y, w, h, 1, !invert);
        d = XftDrawCreate(drw->dpy, drw->drawable, drw->visual, drw->cmap);
        x += lpad;
        w -= lpad;
    }

    usedfont = drw->fonts;
    if (!ellipsis_width && render)
        ellipsis_width = drw_fontset_getwidth(drw, "...");
    while (1)
    {
        ew = ellipsis_len = utf8strlen = 0;
        utf8str = text;
        nextfont = NULL;
        while (*text)
        {
            utf8charlen = utf8decode(text, &utf8codepoint, UTF_SIZ);
            for (curfont = drw->fonts; curfont; curfont = curfont->next)
            {
                charexists =
                    charexists ||
                    XftCharExists(drw->dpy, curfont->xfont, utf8codepoint);
                if (charexists)
                {
                    drw_font_getexts(curfont, text, utf8charlen, &tmpw, NULL);
                    if (ew + ellipsis_width <= w)
                    {
                        /* keep track where the ellipsis still fits */
                        ellipsis_x = x + ew;
                        ellipsis_w = w - ew;
                        ellipsis_len = utf8strlen;
                    }

                    if (ew + tmpw > w)
                    {
                        overflow = 1;
                        /* called from drw_fontset_getwidth_clamp():
                         * it wants the width AFTER the overflow
                         */
                        if (!render)
                            x += tmpw;
                        else
                            utf8strlen = ellipsis_len;
                    }
                    else if (curfont == usedfont)
                    {
                        utf8strlen += utf8charlen;
                        text += utf8charlen;
                        ew += tmpw;
                    }
                    else
                    {
                        nextfont = curfont;
                    }
                    break;
                }
            }

            if (overflow || !charexists || nextfont)
                break;
            else
                charexists = 0;
        }

        if (utf8strlen)
        {
            if (render)
            {
                ty = y + (h - usedfont->h) / 2 + usedfont->xfont->ascent;
                XftDrawStringUtf8(
                    d,
                    &drw->scheme[invert ? ColBg : ColFg],
                    usedfont->xfont,
                    x,
                    ty,
                    (XftChar8*)utf8str,
                    utf8strlen);
            }
            x += ew;
            w -= ew;
        }
        if (render && overflow)
            drw_text(drw, ellipsis_x, y, ellipsis_w, h, 0, "...", invert);

        if (!*text || overflow)
        {
            break;
        }
        else if (nextfont)
        {
            charexists = 0;
            usedfont = nextfont;
        }
        else
        {
            /* Regardless of whether or not a fallback font is found, the
             * character must be drawn. */
            charexists = 1;

            for (i = 0; i < nomatches_len; ++i)
            {
                /* avoid calling XftFontMatch if we know we won't find a match
                 */
                if (utf8codepoint == nomatches.codepoint[i])
                    goto no_match;
            }

            fccharset = FcCharSetCreate();
            FcCharSetAddChar(fccharset, utf8codepoint);

            if (!drw->fonts->pattern)
            {
                /* Refer to the comment in xfont_create for more information. */
                die("the first font in the cache must be loaded from a font string.");
            }

            fcpattern = FcPatternDuplicate(drw->fonts->pattern);
            FcPatternAddCharSet(fcpattern, FC_CHARSET, fccharset);
            FcPatternAddBool(fcpattern, FC_SCALABLE, FcTrue);
            FcPatternAddBool(fcpattern, FC_COLOR, FcFalse);

            FcConfigSubstitute(NULL, fcpattern, FcMatchPattern);
            FcDefaultSubstitute(fcpattern);
            match = XftFontMatch(drw->dpy, drw->screen, fcpattern, &result);

            FcCharSetDestroy(fccharset);
            FcPatternDestroy(fcpattern);

            if (match)
            {
                usedfont = xfont_create(drw, NULL, match);
                if (usedfont &&
                    XftCharExists(drw->dpy, usedfont->xfont, utf8codepoint))
                {
                    for (curfont = drw->fonts; curfont->next;
                         curfont = curfont->next)
                        ; /* NOP */
                    curfont->next = usedfont;
                }
                else
                {
                    xfont_free(usedfont);
                    nomatches.codepoint[++nomatches.idx % nomatches_len] =
                        utf8codepoint;
                no_match:
                    usedfont = drw->fonts;
                }
            }
        }
    }
    if (d)
        XftDrawDestroy(d);

    return x + (render ? w : 0);
}

int drw_text_no_bg(
    Drw* drw,
    int x,
    int y,
    unsigned int w,
    unsigned int h,
    unsigned int lpad,
    const char* text,
    int invert)
{
    int i, ty, ellipsis_x = 0;
    unsigned int tmpw, ew, ellipsis_w = 0, ellipsis_len;
    XftDraw* d = NULL;
    Fnt *usedfont, *curfont, *nextfont;
    int utf8strlen, utf8charlen, render = x || y || w || h;
    long utf8codepoint = 0;
    const char* utf8str;
    FcCharSet* fccharset;
    FcPattern* fcpattern;
    FcPattern* match;
    XftResult result;
    int charexists = 0, overflow = 0;
    /* keep track of a couple codepoints for which we have no match. */
    enum
    {
        nomatches_len = 64
    };
    static struct
    {
        long codepoint[nomatches_len];
        unsigned int idx;
    } nomatches;
    static unsigned int ellipsis_width = 0;

    if (!drw || (render && (!drw->scheme || !w)) || !text || !drw->fonts)
        return 0;

    if (!render)
    {
        w = invert ? invert : ~invert;
    }
    else
    {
        d = XftDrawCreate(drw->dpy, drw->drawable, drw->visual, drw->cmap);
        x += lpad;
        w -= lpad;
    }

    usedfont = drw->fonts;
    if (!ellipsis_width && render)
        ellipsis_width = drw_fontset_getwidth(drw, "...");
    while (1)
    {
        ew = ellipsis_len = utf8strlen = 0;
        utf8str = text;
        nextfont = NULL;
        while (*text)
        {
            utf8charlen = utf8decode(text, &utf8codepoint, UTF_SIZ);
            for (curfont = drw->fonts; curfont; curfont = curfont->next)
            {
                charexists =
                    charexists ||
                    XftCharExists(drw->dpy, curfont->xfont, utf8codepoint);
                if (charexists)
                {
                    drw_font_getexts(curfont, text, utf8charlen, &tmpw, NULL);
                    if (ew + ellipsis_width <= w)
                    {
                        /* keep track where the ellipsis still fits */
                        ellipsis_x = x + ew;
                        ellipsis_w = w - ew;
                        ellipsis_len = utf8strlen;
                    }

                    if (ew + tmpw > w)
                    {
                        overflow = 1;
                        /* called from drw_fontset_getwidth_clamp():
                         * it wants the width AFTER the overflow
                         */
                        if (!render)
                            x += tmpw;
                        else
                            utf8strlen = ellipsis_len;
                    }
                    else if (curfont == usedfont)
                    {
                        utf8strlen += utf8charlen;
                        text += utf8charlen;
                        ew += tmpw;
                    }
                    else
                    {
                        nextfont = curfont;
                    }
                    break;
                }
            }

            if (overflow || !charexists || nextfont)
                break;
            else
                charexists = 0;
        }

        if (utf8strlen)
        {
            if (render)
            {
                ty = y + (h - usedfont->h) / 2 + usedfont->xfont->ascent;
                XftDrawStringUtf8(
                    d,
                    &drw->scheme[invert ? ColBg : ColFg],
                    usedfont->xfont,
                    x,
                    ty,
                    (XftChar8*)utf8str,
                    utf8strlen);
            }
            x += ew;
            w -= ew;
        }
        if (render && overflow)
            drw_text_no_bg(drw, ellipsis_x, y, ellipsis_w, h, 0, "...", invert);

        if (!*text || overflow)
        {
            break;
        }
        else if (nextfont)
        {
            charexists = 0;
            usedfont = nextfont;
        }
        else
        {
            /* Regardless of whether or not a fallback font is found, the
             * character must be drawn. */
            charexists = 1;

            for (i = 0; i < nomatches_len; ++i)
            {
                /* avoid calling XftFontMatch if we know we won't find a match
                 */
                if (utf8codepoint == nomatches.codepoint[i])
                    goto no_match;
            }

            fccharset = FcCharSetCreate();
            FcCharSetAddChar(fccharset, utf8codepoint);

            if (!drw->fonts->pattern)
            {
                /* Refer to the comment in xfont_create for more information. */
                die("the first font in the cache must be loaded from a font string.");
            }

            fcpattern = FcPatternDuplicate(drw->fonts->pattern);
            FcPatternAddCharSet(fcpattern, FC_CHARSET, fccharset);
            FcPatternAddBool(fcpattern, FC_SCALABLE, FcTrue);
            FcPatternAddBool(fcpattern, FC_COLOR, FcFalse);

            FcConfigSubstitute(NULL, fcpattern, FcMatchPattern);
            FcDefaultSubstitute(fcpattern);
            match = XftFontMatch(drw->dpy, drw->screen, fcpattern, &result);

            FcCharSetDestroy(fccharset);
            FcPatternDestroy(fcpattern);

            if (match)
            {
                usedfont = xfont_create(drw, NULL, match);
                if (usedfont &&
                    XftCharExists(drw->dpy, usedfont->xfont, utf8codepoint))
                {
                    for (curfont = drw->fonts; curfont->next;
                         curfont = curfont->next)
                        ; /* NOP */
                    curfont->next = usedfont;
                }
                else
                {
                    xfont_free(usedfont);
                    nomatches.codepoint[++nomatches.idx % nomatches_len] =
                        utf8codepoint;
                no_match:
                    usedfont = drw->fonts;
                }
            }
        }
    }
    if (d)
        XftDrawDestroy(d);

    return x + (render ? w : 0);
}

#define MUL_CHECK_OF

#ifdef DRW_IMG_SUPPORT
Img* drw_img_load(Drw* drw, const char* path, size_t w, size_t h)
{
    Img* img = calloc(1, sizeof(Img));
    if (!img)
        return NULL;

    int initial_w, initial_h, ch;
    uint8_t* data = stbi_load(path, &initial_w, &initial_h, &ch, 4);
    if (!data)
    {
        free(img);
        return NULL;
    }

    const size_t actual_w = w > 0 ? w : (size_t)initial_w;
    const size_t actual_h = h > 0 ? h : (size_t)initial_h;

    size_t tmp = actual_w * actual_h;
    // overflow handling, i dont think actual_w can be 0 here...
    if (tmp / actual_w != actual_h)
    {
        stbi_image_free(data);
        free(img);
        return NULL;
    }

    const size_t image_size = tmp * 4;
    if (image_size / tmp != 4)
    {
        stbi_image_free(data);
        free(img);
        return NULL;
    }

    if (actual_w != (size_t)initial_w || actual_h != (size_t)initial_h)
    {
        uint8_t* resize_data = malloc(image_size);
        if (!resize_data)
        {
            stbi_image_free(data);
            free(img);
            return NULL;
        }

        stbir_resize_uint8_linear(
            data,
            initial_w,
            initial_h,
            0,
            resize_data,
            actual_w,
            actual_h,
            0,
            (stbir_pixel_layout)4);

        /* For some fucking reason the red and blue channels are swapped and
        my understanding is that it's the visual's fault. I tried changing
        the red and blue mask values for the visual but nothing changed. So
        this is my solution that I don't know if is going to work for all
        cases. */
        for (size_t i = 0; i < image_size; i += 4)
        {
            u8 tmp = resize_data[i];
            resize_data[i] = resize_data[i + 2];
            resize_data[i + 2] = tmp;
        }

        stbi_image_free(data);
        data = resize_data;
    }

    img->ximage = XCreateImage(
        drw->dpy,
        drw->visual,
        4 * 8,
        ZPixmap,
        0,
        (char*)data,
        actual_w,
        actual_h,
        32,
        0);
    img->width = actual_w;
    img->height = actual_h;

    /* Keep a copy of the original image for alpha blending */
    img->image_original_data = malloc(image_size);
    if (!img->image_original_data)
    {
        XDestroyImage(img->ximage);
        /** As per the man page :
         * "Note that when the image is created using XCreateImage,
         XGetImage,
         * or XSubImage, the destroy p rocedure that the XDestroyImage
         function
         * calls frees both the image structure and the data pointed to by
         the
         * image structure." So we don't need to free 'data'.
         */
        free(img);
        return NULL;
    }

    memcpy(img->image_original_data, data, image_size);
    return img;
}

void drw_img_free(Img* img)
{
    XDestroyImage(img->ximage);
    free(img->image_original_data);
    free(img);
}

int drw_img(Drw* drw, Img* img, i32 x, i32 y)
{
    XImage* bgimage = XGetImage(
        drw->dpy,
        drw->drawable,
        x,
        y,
        img->width,
        img->height,
        0xFFFFFFFF,
        ZPixmap);

    /* Alpha blending in software cause X11 is bitch */
    for (size_t i = 0; i < img->width * img->height * 4; i += 4)
    {
        // Opaque, no point in blending
        if ((u8)img->image_original_data[i + 3] == 0xFF)
            continue;

        /* FIXME: There's something waiting to go wrong here with the endianness
         */
        float a0 = img->image_original_data[i + 3] / 255.f;
        float a1 = ((u8)bgimage->data[i + 3]) / 255.f;
        float a01 = (1 - a0) * a1 + a0;

        u8 r = ((1 - a0) * a1 * ((u8)bgimage->data[i + 0]) +
                a0 * img->image_original_data[i]) /
               a01;
        u8 g = ((1 - a0) * a1 * ((u8)bgimage->data[i + 1]) +
                a0 * img->image_original_data[i + 1]) /
               a01;
        u8 b = ((1 - a0) * a1 * ((u8)bgimage->data[i + 2]) +
                a0 * img->image_original_data[i + 2]) /
               a01;

        img->ximage->data[i] = r;
        img->ximage->data[i + 1] = g;
        img->ximage->data[i + 2] = b;
        img->ximage->data[i + 3] = a01 * 255;
    }

    /* NOWHERE IN THE FUCKING MANPAGE FOR XGetImage does it mention that
    XDestroyImage needs to be called on the returned XImage to avoid memory
    leaks. NOT THO MENTION the fact that the word Get in XGetImage does not in
    any way indicate allocation>???? Horrible documentation, horrible fortune,
    plenty memory leaks */
    XDestroyImage(bgimage);
    XPutImage(
        drw->dpy,
        drw->drawable,
        drw->gc,
        img->ximage,
        0,
        0,
        x,
        y,
        img->width,
        img->height);
    return 0;
}
#endif // DRW_IMG_SUPPORT

void drw_map(Drw* drw, Window win, int x, int y, unsigned int w, unsigned int h)
{
    if (!drw)
        return;

    XCopyArea(drw->dpy, drw->drawable, win, drw->gc, x, y, w, h, x, y);
    XSync(drw->dpy, False);
}

unsigned int drw_fontset_getwidth(Drw* drw, const char* text)
{
    if (!drw || !drw->fonts || !text)
        return 0;
    return drw_text(drw, 0, 0, 0, 0, 0, text, 0);
}

unsigned int
drw_fontset_getwidth_clamp(Drw* drw, const char* text, unsigned int n)
{
    unsigned int tmp = 0;
    if (drw && drw->fonts && text && n)
        tmp = drw_text(drw, 0, 0, 0, 0, 0, text, n);
    return MIN(n, tmp);
}

void drw_font_getexts(
    Fnt* font,
    const char* text,
    unsigned int len,
    unsigned int* w,
    unsigned int* h)
{
    XGlyphInfo ext;

    if (!font || !text)
        return;

    XftTextExtentsUtf8(font->dpy, font->xfont, (XftChar8*)text, len, &ext);
    if (w)
        *w = ext.xOff;
    if (h)
        *h = font->h;
}

Cur* drw_cur_create(Drw* drw, int shape)
{
    Cur* cur;

    if (!drw || !(cur = ecalloc(1, sizeof(Cur))))
        return NULL;

    cur->cursor = XCreateFontCursor(drw->dpy, shape);

    return cur;
}

void drw_cur_free(Drw* drw, Cur* cursor)
{
    if (!cursor)
        return;

    XFreeCursor(drw->dpy, cursor->cursor);
    free(cursor);
}
