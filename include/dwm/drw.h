/* See LICENSE file for copyright and license details. */

#ifndef DWM_DRW_H
#define DWM_DRW_H

#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <dwm/types.h>

// #define DRW_IMG_SUPPORT

typedef struct
{
    Cursor cursor;
} Cur;

typedef struct Fnt
{
    Display* dpy;
    unsigned int h;
    XftFont* xfont;
    FcPattern* pattern;
    struct Fnt* next;
} Fnt;

enum
{
    ColFg,
    ColBg,
    ColBorder
}; /* Clr scheme index */
typedef XftColor Clr;

typedef struct
{
    unsigned int w, h;
    Display* dpy;
    int screen;
    Window root;
    Visual* visual;
    u32 depth;
    Colormap cmap;
    Drawable drawable;
    GC gc;
    Clr* scheme;
    Fnt* fonts;
    bool use_argb;
} Drw;

#ifdef DRW_IMG_SUPPORT
typedef struct
{
    XImage* ximage;
    /* For alpha blending */
    u8* image_original_data;
    size_t width;
    size_t height;
    u8 channels;
} Img;
#endif // DRW_IMG_SUPPORT

/* Drawable abstraction */
Drw* drw_create(
    Display* dpy, int screen, Window root, unsigned int w, unsigned int h);
void drw_resize(Drw* drw, unsigned int w, unsigned int h);
void drw_free(Drw* drw);

/* Fnt abstraction */
Fnt* drw_fontset_create(Drw* drw, const char* fonts[], size_t fontcount);
void drw_fontset_free(Fnt* set);
unsigned int drw_fontset_getwidth(Drw* drw, const char* text);
unsigned int
drw_fontset_getwidth_clamp(Drw* drw, const char* text, unsigned int n);
void drw_font_getexts(
    Fnt* font,
    const char* text,
    unsigned int len,
    unsigned int* w,
    unsigned int* h);

/* Colorscheme abstraction */
void drw_clr_create(Drw* drw, Clr* dest, const char* clrname);
Clr* drw_scm_create(Drw* drw, const char* clrnames[], size_t clrcount);

/* Cursor abstraction */
Cur* drw_cur_create(Drw* drw, int shape);
void drw_cur_free(Drw* drw, Cur* cursor);

/* Drawing context manipulation */
void drw_setfontset(Drw* drw, Fnt* set);
void drw_setscheme(Drw* drw, Clr* scm);

/* Drawing functions */
void drw_rect(
    Drw* drw,
    int x,
    int y,
    unsigned int w,
    unsigned int h,
    int filled,
    int invert);
int drw_text(
    Drw* drw,
    int x,
    int y,
    unsigned int w,
    unsigned int h,
    unsigned int lpad,
    const char* text,
    int invert);
int drw_text_no_bg(
    Drw* drw,
    int x,
    int y,
    unsigned int w,
    unsigned int h,
    unsigned int lpad,
    const char* text,
    int invert);

void drw_circle(
    Drw* drw, int x, int y, unsigned int rad, int filled, int invert);
void drw_circle_bordered(
    Drw* drw,
    int x,
    int y,
    unsigned int w,
    unsigned int h,
    int filled,
    int invert);

#ifdef DRW_IMG_SUPPORT
/**
 * Load an image to use with drw_img. The image is automatically alpha blended
 * in software.
 *
 * path: Path on disk to the image file.
 * w: The desired width of the image. 0 if it should mentain it's original width
 * h: The desired height of the image. 0 if it should mentain it's original
 * height
 */
Img* drw_img_load(Drw* drw, const char* path, size_t w, size_t h);

int drw_img(Drw* drw, Img* img, i32 x, i32 y);

void drw_img_free(Img* img);
#endif // DRW_IMG_SUPPORT

/* Map functions */
void drw_map(
    Drw* drw, Window win, int x, int y, unsigned int w, unsigned int h);

#endif // !DWM_DRW_H
