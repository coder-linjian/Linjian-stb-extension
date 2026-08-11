/* stb_image_paint.h - v1.1.0 - image drawing primitives for stb_image
 *
 * Copyright (c) 2026 Linjian Studio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ============================================================================
 *
 * USAGE:
 *
 *   Define STB_IMAGE_PAINT_IMPLEMENTATION in exactly ONE C file before including
 *   this header to generate the implementation.
 *
 *   #define STB_IMAGE_PAINT_IMPLEMENTATION
 *   #include "stb_image_paint.h"
 *
 *   All functions operate on 8?bit unsigned interleaved pixel data.
 *   The caller is responsible for allocating image buffers of sufficient size.
 *
 *   Optional TrueType support: define STBIP_USE_TRUETYPE and include
 *   "stb_truetype.h" in your project. See stbip_ttf_load() for details.
 *
 *   Built?in bitmap font: 8¡Á8 glyphs are horizontally stretched to 16¡Á8 for
 *   better readability. The font data array is named '_stbip_bitmap_font'.
 *
 *   TrueType text rendering:
 *     - stbip_draw_text_ttf_utf8()  : input is UTF?8 encoded (recommended for
 *                                      cross?platform portability).
 *     - stbip_draw_text_ttf_ansi()  : input is ANSI (system locale encoding).
 *                                      Requires setlocale(LC_ALL, "") before use.
 *                                      This function converts to UTF?8 internally
 *                                      and then calls stbip_draw_text_ttf_utf8().
 *     - stbip_draw_text_ttf         : macro alias for stbip_draw_text_ttf_ansi
 *                                      (default to ANSI for convenience on
 *                                      Chinese/Windows systems).
 *
 *   NEW in v1.1.0:
 *     - Alpha blending: enable via stbip_enable_alpha_blend(1). All drawing
 *       functions will use the alpha component of the color array (if image has
 *       4 channels) to blend with the destination pixel.
 *     - Antialiased lines: stbip_draw_line_aa() implements Wu's algorithm for
 *       smooth lines.
 *     - Bezier curves: stbip_draw_bezier() draws cubic Bezier curves with
 *       optional thickness and antialiasing.
 *
 * ============================================================================
 */

#ifndef STBIP_IMAGE_PAINT_H_INCLUDED
#define STBIP_IMAGE_PAINT_H_INCLUDED

#define STBIP_VERSION 110

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>  /* for size_t */

typedef unsigned char stbi_uc; 

/* ---- user?configurable macros ------------------------------------------ */

#ifndef STBIPDEF
# ifdef STBIP_STATIC
#  define STBIPDEF static
# else
#  define STBIPDEF extern
# endif
#endif

/* ---- error codes -------------------------------------------------------- */

typedef enum {
    STBIP_ERROR_NONE = 0,
    STBIP_ERROR_INVALID_IMAGE,          /* data is NULL, or width/height/channels invalid */
    STBIP_ERROR_INVALID_PARAM,          /* e.g. color is NULL, negative size */
    STBIP_ERROR_OUT_OF_MEMORY,          /* memory allocation failed */
    STBIP_ERROR_TOO_MANY_VERTICES,      /* polygon has more vertices than internal limit */
    STBIP_ERROR_FLOOD_STACK_OVERFLOW,   /* flood fill stack size exceeded */
    STBIP_ERROR_UNSUPPORTED,            /* unsupported operation (e.g. TrueType not enabled) */
    STBIP_ERROR_FILE_IO,                /* file open/read error (TrueType) */
    STBIP_ERROR_FONT_NOT_LOADED,        /* no current font set or font load failed */
    STBIP_ERROR_TOO_MANY_FONTS,         /* exceeded maximum number of loaded fonts */
    STBIP_ERROR_ENCODING,               /* ANSI to wide char conversion failed */
} stbip_error;

/* Returns the last error code that occurred. */
STBIPDEF stbip_error stbip_get_last_error(void);

/* ---- public API --------------------------------------------------------- */

/* --- Basic drawing (v1.0.0) --- */

/* Draw a single pixel at (x,y). Clipped to image bounds.
   Returns 1 on success, 0 on error. */
STBIPDEF int stbip_draw_pixel(stbi_uc *data, int w, int h, int c,
                              int x, int y, const stbi_uc color[4]);

/* Draw a line from (x0,y0) to (x1,y1) using Bresenham's algorithm.
   Clips the line to image bounds. Returns 1 on success. */
STBIPDEF int stbip_draw_line(stbi_uc *data, int w, int h, int c,
                             int x0, int y0, int x1, int y1,
                             const stbi_uc color[4]);

/* Draw a rectangle with top?left corner (x,y), size (rect_w, rect_h).
   If fill == 1, fills the interior; otherwise draws a border of thickness.
   Thickness is in pixels. If thickness <= 0, it is treated as 1.
   Clips to image bounds. Returns 1 on success. */
STBIPDEF int stbip_draw_rect(stbi_uc *data, int w, int h, int c,
                             int x, int y, int rect_w, int rect_h,
                             const stbi_uc color[4], int fill, int thickness);

/* Draw a circle centered at (cx,cy) with given radius.
   If fill == 1, fills the circle; otherwise draws its outline (1 pixel wide).
   Returns 1 on success. */
STBIPDEF int stbip_draw_circle(stbi_uc *data, int w, int h, int c,
                               int cx, int cy, int radius,
                               const stbi_uc color[4], int fill);

/* Draw a polygon defined by an array of vertices.
   vertices: pointer to int array of length 2*num_vertices (x0,y0,x1,y1,...).
   If fill == 1, fills the polygon using scanline filling; otherwise draws its outline.
   Returns 1 on success, 0 on error (e.g. too many vertices). */
STBIPDEF int stbip_draw_polygon(stbi_uc *data, int w, int h, int c,
                                const int *vertices, int num_vertices,
                                const stbi_uc color[4], int fill);

/* Draw an ASCII text string at (x,y) using a built?in 16¡Á8 bitmap font
   (horizontally stretched from 8¡Á8 glyphs). Only characters 32?126 are
   supported; others are skipped. The text is drawn with the given color
   (opaque, no background). Returns 1 on success. */
STBIPDEF int stbip_draw_text(stbi_uc *data, int w, int h, int c,
                             int x, int y, const char *text,
                             const stbi_uc color[4]);

/* Perform a flood fill starting from (x,y) with a new color.
   tolerance: allowed difference per channel (0 means exact match).
   adjacency: 4 or 8 (4?connected or 8?connected).
   Returns 1 on success, 0 on error (e.g. stack overflow). */
STBIPDEF int stbip_flood_fill(stbi_uc *data, int w, int h, int c,
                              int x, int y, const stbi_uc new_color[4],
                              int tolerance, int adjacency);

/* Fill the entire image with a solid color. Returns 1 on success. */
STBIPDEF int stbip_clear_image(stbi_uc *data, int w, int h, int c,
                               const stbi_uc color[4]);

/* Alias for stbip_clear_image (kept for backward compatibility) */
#define stbip_clear stbip_clear_image

/* --- New in v1.1.0: Alpha blending control --- */
/* Enable or disable global alpha blending.
   When enabled (1), all drawing operations will blend the source color's
   alpha channel with the destination pixel (only valid for 4?channel images).
   Default is disabled (0) for backward compatibility. */
STBIPDEF void stbip_enable_alpha_blend(int enable);

/* --- New in v1.1.0: Antialiased line --- */
/* Draw an antialiased line using Wu's algorithm.
   The line is drawn with the given color and thickness (in pixels).
   If thickness > 1, the line is drawn as a thick antialiased line (using
   multiple parallel lines). Returns 1 on success, 0 on error. */
STBIPDEF int stbip_draw_line_aa(stbi_uc *data, int w, int h, int c,
                                int x0, int y0, int x1, int y1,
                                const stbi_uc color[4], int thickness);

/* --- New in v1.1.0: Cubic Bezier curve --- */
/* Draw a cubic Bezier curve defined by four control points (x0,y0) start,
   (x1,y1) control1, (x2,y2) control2, (x3,y3) end.
   The curve is approximated by line segments and rendered with antialiasing
   (via stbip_draw_line_aa) with the given thickness.
   Returns 1 on success, 0 on error. */
STBIPDEF int stbip_draw_bezier(stbi_uc *data, int w, int h, int c,
                               int x0, int y0, int x1, int y1,
                               int x2, int y2, int x3, int y3,
                               const stbi_uc color[4], int thickness);

/* ---- TrueType text rendering (optional) ---- */
#ifdef STBIP_USE_TRUETYPE
/* Load a TrueType font from file and store it internally.
   Returns font index (>= 0) on success, or -1 on error (check error code). */
STBIPDEF int stbip_ttf_load(const char *path);

/* Set the current font for subsequent text drawing.
   'font_index' must be a value returned by stbip_ttf_load().
   Returns 1 on success, 0 on error (e.g. invalid index). */
STBIPDEF int stbip_ttf_set_current(int font_index);

/* Free all loaded TrueType fonts and reset the current font state. */
STBIPDEF void stbip_ttf_done(void);

/* Draw text using the currently loaded TrueType font.
   The font must have been loaded and set as current beforehand.
   'font_size' is the desired pixel height of the text.
   The input string is expected to be UTF?8 encoded. Unicode characters
   are supported if the font contains the required glyphs.
   Returns 1 on success, 0 on error (e.g. no current font). */
STBIPDEF int stbip_draw_text_ttf_utf8(stbi_uc *data, int w, int h, int c,
                                      int x, int y, const char *text,
                                      const stbi_uc color[4], float font_size);

/* Draw text using the currently loaded TrueType font, with input string
   encoded in the system's ANSI code page (e.g. GBK on Windows, locale?dependent
   on other platforms). This function uses mbstowcs() to convert the string
   to wide characters, then converts to UTF?8 and calls stbip_draw_text_ttf_utf8().
   You must have set the locale correctly (e.g. setlocale(LC_ALL, "")).
   Returns 1 on success, 0 on error (e.g. conversion failure). */
STBIPDEF int stbip_draw_text_ttf_ansi(stbi_uc *data, int w, int h, int c,
                                      int x, int y, const char *text,
                                      const stbi_uc color[4], float font_size);

/* For convenience, stbip_draw_text_ttf defaults to the ANSI version.
   If you need UTF?8, call stbip_draw_text_ttf_utf8 directly. */
#define stbip_draw_text_ttf stbip_draw_text_ttf_ansi

#endif /* STBIP_USE_TRUETYPE */

#ifdef __cplusplus
}
#endif

/* ---- implementation ----------------------------------------------------- */
#ifdef STB_IMAGE_PAINT_IMPLEMENTATION

#include <string.h>  /* for memcpy, memset */
#include <stdlib.h>  /* for abs, malloc, free */
#include <math.h>    /* for sqrtf, fabsf */

#ifdef STBIP_USE_TRUETYPE
# include <stdio.h>   /* for fopen, fseek, ftell, fread, fclose */
# include <wchar.h>   /* for mbstowcs */
# include "stb_truetype.h"
#endif

/* ---- internal helpers --------------------------------------------------- */

/* clamp value between min and max */
static int _stbip_clamp(int v, int min, int max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

/* Global alpha blending enable flag (default: 0 = disabled) */
static int _stbip_alpha_blend_enabled = 0;

/* set pixel with optional alpha blending (if enabled and c == 4) */
static void _stbip_set_pixel(stbi_uc *data, int w, int h, int c,
                             int x, int y, const stbi_uc color[4]) {
    if (x < 0 || x >= w || y < 0 || y >= h) return;
    int idx = (y * w + x) * c;
    if (_stbip_alpha_blend_enabled && c == 4) {
        unsigned char src[4];
        unsigned char dst[4];
        for (int i = 0; i < 4; ++i) src[i] = color[i];
        for (int i = 0; i < 4; ++i) dst[i] = data[idx + i];
        /* blend source over destination using source alpha */
        float a = src[3] / 255.0f;
        float inva = 1.0f - a;
        for (int i = 0; i < 3; ++i) {
            data[idx + i] = (unsigned char)(a * src[i] + inva * dst[i] + 0.5f);
        }
        data[idx + 3] = (unsigned char)(src[3] + dst[3] * inva + 0.5f); /* blend alpha as well */
    } else {
        for (int i = 0; i < c; ++i) {
            data[idx + i] = color[i];
        }
    }
}

/* get pixel; returns 1 if valid, 0 otherwise */
static int _stbip_get_pixel(const stbi_uc *data, int w, int h, int c,
                            int x, int y, stbi_uc out[4]) {
    if (x < 0 || x >= w || y < 0 || y >= h) return 0;
    int idx = (y * w + x) * c;
    for (int i = 0; i < c; ++i) out[i] = data[idx + i];
    return 1;
}

/* swap two ints */
static void _stbip_swap_int(int *a, int *b) {
    int t = *a; *a = *b; *b = t;
}

/* ---- UTF?8 decoder (internal) ------------------------------------------ */

/* Decodes one Unicode codepoint from a UTF?8 string.
   Returns the codepoint, and advances the pointer 'p' past the consumed bytes.
   If a decoding error occurs, returns 0xFFFD (U+FFFD) and advances by 1 byte. */
static int _stbip_utf8_decode(const char **p) {
    const unsigned char *s = (const unsigned char*)(*p);
    if (!s[0]) return 0;  /* end of string */

    int cp = 0;
    int bytes = 0;

    /* Determine length of UTF?8 sequence */
    if (s[0] < 0x80) {
        cp = s[0];
        bytes = 1;
    } else if ((s[0] & 0xE0) == 0xC0) {  /* 2?byte */
        if ((s[1] & 0xC0) != 0x80) goto error;
        cp = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
        bytes = 2;
    } else if ((s[0] & 0xF0) == 0xE0) {  /* 3?byte */
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) goto error;
        cp = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        bytes = 3;
    } else if ((s[0] & 0xF8) == 0xF0) {  /* 4?byte */
        if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) goto error;
        cp = ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        bytes = 4;
    } else {
        goto error;
    }

    /* Check for overlong and surrogate halves */
    if (bytes == 2 && cp < 0x80) goto error;
    if (bytes == 3 && cp < 0x800) goto error;
    if (bytes == 4 && cp < 0x10000) goto error;
    if (cp >= 0xD800 && cp <= 0xDFFF) goto error;
    if (cp > 0x10FFFF) goto error;

    *p += bytes;
    return cp;

error:
    *p += 1;  /* skip invalid byte */
    return 0xFFFD;  /* replacement character */
}

/* ---- global error state ------------------------------------------------- */

static stbip_error _stbip_last_error = STBIP_ERROR_NONE;

stbip_error stbip_get_last_error(void) {
    return _stbip_last_error;
}

static void _stbip_set_error(stbip_error err) {
    _stbip_last_error = err;
}

/* ---- draw_pixel --------------------------------------------------------- */

STBIPDEF int stbip_draw_pixel(stbi_uc *data, int w, int h, int c,
                              int x, int y, const stbi_uc color[4]) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !color) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    _stbip_set_pixel(data, w, h, c, x, y, color);
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- draw_line ---------------------------------------------------------- */

STBIPDEF int stbip_draw_line(stbi_uc *data, int w, int h, int c,
                             int x0, int y0, int x1, int y1,
                             const stbi_uc color[4]) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !color) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    /* Bresenham's line algorithm */
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        _stbip_set_pixel(data, w, h, c, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- fill rectangle (internal) ------------------------------------------ */

static void _stbip_fill_rect(stbi_uc *data, int w, int h, int c,
                             int x0, int y0, int x1, int y1,
                             const stbi_uc color[4]) {
    /* ensure x0 <= x1, y0 <= y1 */
    if (x0 > x1) { int t = x0; x0 = x1; x1 = t; }
    if (y0 > y1) { int t = y0; y0 = y1; y1 = t; }
    /* clip to image bounds */
    x0 = _stbip_clamp(x0, 0, w - 1);
    x1 = _stbip_clamp(x1, 0, w - 1);
    y0 = _stbip_clamp(y0, 0, h - 1);
    y1 = _stbip_clamp(y1, 0, h - 1);
    if (x0 > x1 || y0 > y1) return;
    for (int y = y0; y <= y1; ++y) {
        int base = (y * w + x0) * c;
        int len = (x1 - x0 + 1) * c;
        for (int i = 0; i < len; i += c) {
            for (int ch = 0; ch < c; ++ch) {
                data[base + i + ch] = color[ch];
            }
        }
    }
}

/* ---- draw_rect ---------------------------------------------------------- */

STBIPDEF int stbip_draw_rect(stbi_uc *data, int w, int h, int c,
                             int x, int y, int rect_w, int rect_h,
                             const stbi_uc color[4], int fill, int thickness) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !color ||
        rect_w <= 0 || rect_h <= 0) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    if (thickness <= 0) thickness = 1;
    int x1 = x + rect_w - 1;
    int y1 = y + rect_h - 1;
    if (fill) {
        _stbip_fill_rect(data, w, h, c, x, y, x1, y1, color);
    } else {
        /* draw four border rectangles (or use lines with thickness) */
        /* top border */
        _stbip_fill_rect(data, w, h, c, x, y, x1, y + thickness - 1, color);
        /* bottom border */
        _stbip_fill_rect(data, w, h, c, x, y1 - thickness + 1, x1, y1, color);
        /* left border (excluding corners already drawn) */
        _stbip_fill_rect(data, w, h, c, x, y + thickness, x + thickness - 1, y1 - thickness, color);
        /* right border */
        _stbip_fill_rect(data, w, h, c, x1 - thickness + 1, y + thickness, x1, y1 - thickness, color);
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- draw_circle -------------------------------------------------------- */

STBIPDEF int stbip_draw_circle(stbi_uc *data, int w, int h, int c,
                               int cx, int cy, int radius,
                               const stbi_uc color[4], int fill) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !color || radius < 0) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    if (radius == 0) {
        _stbip_set_pixel(data, w, h, c, cx, cy, color);
        _stbip_set_error(STBIP_ERROR_NONE);
        return 1;
    }
    if (fill) {
        /* for each row, find left and right intersection of circle */
        for (int y = -radius; y <= radius; ++y) {
            int dy = y;
            int dx = (int)(sqrtf((float)(radius * radius - dy * dy)));
            int x0 = cx - dx, x1 = cx + dx;
            /* draw horizontal line from x0 to x1 at cy + y */
            int yy = cy + y;
            if (yy < 0 || yy >= h) continue;
            int x0_clamped = _stbip_clamp(x0, 0, w - 1);
            int x1_clamped = _stbip_clamp(x1, 0, w - 1);
            if (x0_clamped > x1_clamped) continue;
            int base = (yy * w + x0_clamped) * c;
            int len = (x1_clamped - x0_clamped + 1) * c;
            for (int i = 0; i < len; i += c) {
                for (int ch = 0; ch < c; ++ch) {
                    data[base + i + ch] = color[ch];
                }
            }
        }
    } else {
        /* outline using midpoint circle algorithm */
        int x = radius, y = 0;
        int err = 1 - x;
        while (x >= y) {
            /* plot 8 symmetric points */
            _stbip_set_pixel(data, w, h, c, cx + x, cy + y, color);
            _stbip_set_pixel(data, w, h, c, cx + y, cy + x, color);
            _stbip_set_pixel(data, w, h, c, cx - y, cy + x, color);
            _stbip_set_pixel(data, w, h, c, cx - x, cy + y, color);
            _stbip_set_pixel(data, w, h, c, cx - x, cy - y, color);
            _stbip_set_pixel(data, w, h, c, cx - y, cy - x, color);
            _stbip_set_pixel(data, w, h, c, cx + y, cy - x, color);
            _stbip_set_pixel(data, w, h, c, cx + x, cy - y, color);
            y++;
            if (err < 0) {
                err += 2 * y + 1;
            } else {
                x--;
                err += 2 * (y - x) + 1;
            }
        }
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- draw_polygon ------------------------------------------------------- */

#define STBIP_MAX_POLY_VERTICES 256

/* draw polygon outline by connecting vertices */
static void _stbip_draw_polygon_outline(stbi_uc *data, int w, int h, int c,
                                        const int *verts, int n,
                                        const stbi_uc color[4]) {
    for (int i = 0; i < n; ++i) {
        int x0 = verts[2*i], y0 = verts[2*i+1];
        int x1 = verts[2*((i+1)%n)], y1 = verts[2*((i+1)%n)+1];
        stbip_draw_line(data, w, h, c, x0, y0, x1, y1, color);
    }
}

/* scanline fill for polygon */
static void _stbip_fill_polygon(stbi_uc *data, int w, int h, int c,
                                const int *verts, int n,
                                const stbi_uc color[4]) {
    /* find y range */
    int ymin = verts[1], ymax = verts[1];
    for (int i = 1; i < n; ++i) {
        int y = verts[2*i+1];
        if (y < ymin) ymin = y;
        if (y > ymax) ymax = y;
    }
    ymin = _stbip_clamp(ymin, 0, h - 1);
    ymax = _stbip_clamp(ymax, 0, h - 1);
    if (ymin > ymax) return;

    /* for each scanline, compute intersections */
    for (int y = ymin; y <= ymax; ++y) {
        int intersections[STBIP_MAX_POLY_VERTICES];
        int num_int = 0;
        for (int i = 0; i < n; ++i) {
            int x0 = verts[2*i], y0 = verts[2*i+1];
            int x1 = verts[2*((i+1)%n)], y1 = verts[2*((i+1)%n)+1];
            /* check if edge crosses this scanline (y strictly between or at endpoints with proper handling) */
            int ylo = y0 < y1 ? y0 : y1;
            int yhi = y0 > y1 ? y0 : y1;
            if (y < ylo || y > yhi) continue;
            if (yhi == ylo) continue; /* horizontal edge */
            /* compute x intersection */
            float t = (float)(y - y0) / (y1 - y0);
            int x = (int)(x0 + t * (x1 - x0) + 0.5f);
            /* avoid duplicate intersections at vertices (use half?open rule) */
            /* skip if y == yhi and edge goes upward? We'll use simple: if y == yhi, we might get duplicate, but we can ignore if we sort */
            /* To keep simple, we add all, then sort and pair */
            if (num_int < STBIP_MAX_POLY_VERTICES) {
                intersections[num_int++] = x;
            }
        }
        /* sort intersections */
        for (int i = 0; i < num_int - 1; ++i) {
            for (int j = i + 1; j < num_int; ++j) {
                if (intersections[i] > intersections[j]) {
                    int t = intersections[i]; intersections[i] = intersections[j]; intersections[j] = t;
                }
            }
        }
        /* draw pairs */
        for (int i = 0; i + 1 < num_int; i += 2) {
            int x0 = _stbip_clamp(intersections[i], 0, w - 1);
            int x1 = _stbip_clamp(intersections[i+1], 0, w - 1);
            if (x0 > x1) { int t = x0; x0 = x1; x1 = t; }
            if (x0 > x1) continue;
            int base = (y * w + x0) * c;
            int len = (x1 - x0 + 1) * c;
            for (int j = 0; j < len; j += c) {
                for (int ch = 0; ch < c; ++ch) {
                    data[base + j + ch] = color[ch];
                }
            }
        }
    }
}

STBIPDEF int stbip_draw_polygon(stbi_uc *data, int w, int h, int c,
                                const int *vertices, int num_vertices,
                                const stbi_uc color[4], int fill) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !vertices || !color ||
        num_vertices < 3) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    if (num_vertices > STBIP_MAX_POLY_VERTICES) {
        _stbip_set_error(STBIP_ERROR_TOO_MANY_VERTICES);
        return 0;
    }
    if (fill) {
        _stbip_fill_polygon(data, w, h, c, vertices, num_vertices, color);
    } else {
        _stbip_draw_polygon_outline(data, w, h, c, vertices, num_vertices, color);
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- draw_text (bitmap font, 16¡Á8 stretched) --------------------------- */

/* 8¡Á8 bitmap font for ASCII 32..126.
   Each character is 8 bytes, each byte is a row (LSB leftmost).
   The font is horizontally stretched to 16 pixels wide at drawing time.
   The data array is named '_stbip_bitmap_font'. */
static const unsigned char _stbip_bitmap_font[95][8] = {
    /* 32 ' ' */ {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    /* 33 '!' */ {0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00},
    /* 34 '"' */ {0x36,0x36,0x36,0x00,0x00,0x00,0x00,0x00},
    /* 35 '#' */ {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00},
    /* 36 '$' */ {0x08,0x3E,0x28,0x3E,0x0A,0x3E,0x08,0x00},
    /* 37 '%' */ {0x60,0x66,0x0C,0x18,0x30,0x66,0x06,0x00},
    /* 38 '&' */ {0x38,0x6C,0x6C,0x38,0x6D,0x66,0x3B,0x00},
    /* 39 ''' */ {0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00},
    /* 40 '(' */ {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00},
    /* 41 ')' */ {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00},
    /* 42 '*' */ {0x00,0x36,0x1C,0x7F,0x1C,0x36,0x00,0x00},
    /* 43 '+' */ {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00},
    /* 44 ',' */ {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30},
    /* 45 '-' */ {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00},
    /* 46 '.' */ {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00},
    /* 47 '/' */ {0x00,0x06,0x0C,0x18,0x30,0x60,0x00,0x00},
    /* 48 '0' */ {0x3C,0x66,0x6E,0x7E,0x76,0x66,0x3C,0x00},
    /* 49 '1' */ {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00},
    /* 50 '2' */ {0x3C,0x66,0x06,0x0C,0x18,0x30,0x7E,0x00},
    /* 51 '3' */ {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00},
    /* 52 '4' */ {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00},
    /* 53 '5' */ {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00},
    /* 54 '6' */ {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00},
    /* 55 '7' */ {0x7E,0x06,0x0C,0x18,0x30,0x30,0x30,0x00},
    /* 56 '8' */ {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00},
    /* 57 '9' */ {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00},
    /* 58 ':' */ {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00},
    /* 59 ';' */ {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30},
    /* 60 '<' */ {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00},
    /* 61 '=' */ {0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00},
    /* 62 '>' */ {0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00},
    /* 63 '?' */ {0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00},
    /* 64 '@' */ {0x3C,0x66,0x6E,0x6E,0x6E,0x60,0x3E,0x00},
    /* 65 'A' */ {0x3C,0x66,0x66,0x7E,0x66,0x66,0x66,0x00},
    /* 66 'B' */ {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00},
    /* 67 'C' */ {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00},
    /* 68 'D' */ {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00},
    /* 69 'E' */ {0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00},
    /* 70 'F' */ {0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00},
    /* 71 'G' */ {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00},
    /* 72 'H' */ {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00},
    /* 73 'I' */ {0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00},
    /* 74 'J' */ {0x3E,0x0C,0x0C,0x0C,0x0C,0x6C,0x38,0x00},
    /* 75 'K' */ {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00},
    /* 76 'L' */ {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00},
    /* 77 'M' */ {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00},
    /* 78 'N' */ {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00},
    /* 79 'O' */ {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    /* 80 'P' */ {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00},
    /* 81 'Q' */ {0x3C,0x66,0x66,0x66,0x66,0x3C,0x0E,0x00},
    /* 82 'R' */ {0x7C,0x66,0x66,0x7C,0x78,0x6C,0x66,0x00},
    /* 83 'S' */ {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00},
    /* 84 'T' */ {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00},
    /* 85 'U' */ {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    /* 86 'V' */ {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00},
    /* 87 'W' */ {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},
    /* 88 'X' */ {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00},
    /* 89 'Y' */ {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00},
    /* 90 'Z' */ {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00},
    /* 91 '[' */ {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00},
    /* 92 '\' */ {0x00,0x60,0x30,0x18,0x0C,0x06,0x00,0x00},
    /* 93 ']' */ {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00},
    /* 94 '^' */ {0x18,0x3C,0x66,0x00,0x00,0x00,0x00,0x00},
    /* 95 '_' */ {0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00},
    /* 96 '`' */ {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00},
    /* 97 'a' */ {0x00,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00},
    /* 98 'b' */ {0x60,0x60,0x7C,0x66,0x66,0x66,0x7C,0x00},
    /* 99 'c' */ {0x00,0x00,0x3C,0x60,0x60,0x60,0x3C,0x00},
    /*100 'd' */ {0x06,0x06,0x3E,0x66,0x66,0x66,0x3E,0x00},
    /*101 'e' */ {0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00},
    /*102 'f' */ {0x1C,0x30,0x30,0x7C,0x30,0x30,0x30,0x00},
    /*103 'g' */ {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x7C},
    /*104 'h' */ {0x60,0x60,0x7C,0x66,0x66,0x66,0x66,0x00},
    /*105 'i' */ {0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00},
    /*106 'j' */ {0x06,0x00,0x06,0x06,0x06,0x66,0x3C,0x00},
    /*107 'k' */ {0x60,0x60,0x66,0x6C,0x78,0x6C,0x66,0x00},
    /*108 'l' */ {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00},
    /*109 'm' */ {0x00,0x00,0x76,0x7F,0x6B,0x6B,0x6B,0x00},
    /*110 'n' */ {0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00},
    /*111 'o' */ {0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00},
    /*112 'p' */ {0x00,0x00,0x7C,0x66,0x66,0x7C,0x60,0x60},
    /*113 'q' */ {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x06},
    /*114 'r' */ {0x00,0x00,0x7C,0x66,0x60,0x60,0x60,0x00},
    /*115 's' */ {0x00,0x00,0x3C,0x60,0x3C,0x06,0x7C,0x00},
    /*116 't' */ {0x18,0x18,0x7E,0x18,0x18,0x18,0x0E,0x00},
    /*117 'u' */ {0x00,0x00,0x66,0x66,0x66,0x66,0x3E,0x00},
    /*118 'v' */ {0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00},
    /*119 'w' */ {0x00,0x00,0x6B,0x6B,0x6B,0x7F,0x36,0x00},
    /*120 'x' */ {0x00,0x00,0x66,0x3C,0x18,0x3C,0x66,0x00},
    /*121 'y' */ {0x00,0x00,0x66,0x66,0x66,0x3E,0x06,0x7C},
    /*122 'z' */ {0x00,0x00,0x7E,0x0C,0x18,0x30,0x7E,0x00},
    /*123 '{' */ {0x0E,0x18,0x18,0x30,0x18,0x18,0x0E,0x00},
    /*124 '|' */ {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00},
    /*125 '}' */ {0x70,0x18,0x18,0x0C,0x18,0x18,0x70,0x00},
    /*126 '~' */ {0x32,0x4C,0x00,0x00,0x00,0x00,0x00,0x00}
};

STBIPDEF int stbip_draw_text(stbi_uc *data, int w, int h, int c,
                             int x, int y, const char *text,
                             const stbi_uc color[4]) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !text || !color) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    int cursor_x = x;
    int cursor_y = y;
    for (const char *p = text; *p; ++p) {
        unsigned char ch = (unsigned char)(*p);
        if (ch < 32 || ch > 126) continue; /* skip unsupported */
        const unsigned char *glyph = _stbip_bitmap_font[ch - 32];
        for (int row = 0; row < 8; ++row) {
            unsigned char bits = glyph[row];
            int py = cursor_y + row;
            if (py < 0 || py >= h) continue;
            for (int col = 0; col < 8; ++col) {
                if (bits & (1 << (7 - col))) {
                    int px = cursor_x + col * 2;      /* left pixel */
                    _stbip_set_pixel(data, w, h, c, px, py, color);
                    _stbip_set_pixel(data, w, h, c, px + 1, py, color);  /* right pixel */
                }
            }
        }
        cursor_x += 16;   /* width is doubled to 16 pixels */
        if (cursor_x >= w) break;
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- flood fill --------------------------------------------------------- */

#define STBIP_FLOOD_STACK_SIZE 4096

typedef struct {
    int x, y;
} _stbip_flood_node;

STBIPDEF int stbip_flood_fill(stbi_uc *data, int w, int h, int c,
                              int x, int y, const stbi_uc new_color[4],
                              int tolerance, int adjacency) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !new_color) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    if (x < 0 || x >= w || y < 0 || y >= h) {
        _stbip_set_error(STBIP_ERROR_INVALID_PARAM);
        return 0;
    }
    /* get seed color */
    stbi_uc old_color[4];
    if (!_stbip_get_pixel(data, w, h, c, x, y, old_color)) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    /* check if seed already matches new color */
    int same = 1;
    for (int i = 0; i < c; ++i) {
        if (old_color[i] != new_color[i]) { same = 0; break; }
    }
    if (same) {
        _stbip_set_error(STBIP_ERROR_NONE);
        return 1; /* nothing to do */
    }
    /* stack */
    _stbip_flood_node stack[STBIP_FLOOD_STACK_SIZE];
    int top = -1;
    /* push seed */
    if (top + 1 < STBIP_FLOOD_STACK_SIZE) {
        stack[++top] = (_stbip_flood_node){x, y};   /* fixed: removed 'struct' */
    } else {
        _stbip_set_error(STBIP_ERROR_FLOOD_STACK_OVERFLOW);
        return 0;
    }
    /* directions based on adjacency */
    int dirs[8][2] = {{0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
    int num_dirs = (adjacency == 8) ? 8 : 4;
    while (top >= 0) {
        _stbip_flood_node cur = stack[top--];
        int cx = cur.x, cy = cur.y;
        /* set pixel to new color */
        _stbip_set_pixel(data, w, h, c, cx, cy, new_color);
        /* examine neighbors */
        for (int d = 0; d < num_dirs; ++d) {
            int nx = cx + dirs[d][0];
            int ny = cy + dirs[d][1];
            if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
            stbi_uc pixel[4];
            if (!_stbip_get_pixel(data, w, h, c, nx, ny, pixel)) continue;
            /* check if pixel matches old_color within tolerance */
            int match = 1;
            for (int i = 0; i < c; ++i) {
                int diff = pixel[i] - old_color[i];
                if (diff < 0) diff = -diff;
                if (diff > tolerance) { match = 0; break; }
            }
            if (match) {
                /* push neighbor if stack not full */
                if (top + 1 < STBIP_FLOOD_STACK_SIZE) {
                    stack[++top] = (_stbip_flood_node){nx, ny}; /* fixed */
                } else {
                    _stbip_set_error(STBIP_ERROR_FLOOD_STACK_OVERFLOW);
                    return 0;
                }
            }
        }
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- clear_image -------------------------------------------------------- */

STBIPDEF int stbip_clear_image(stbi_uc *data, int w, int h, int c,
                               const stbi_uc color[4]) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !color) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    int total_pixels = w * h;
    for (int i = 0; i < total_pixels; ++i) {
        int base = i * c;
        for (int ch = 0; ch < c; ++ch) {
            data[base + ch] = color[ch];
        }
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- New in v1.1.0: Alpha blending control ----------------------------- */

STBIPDEF void stbip_enable_alpha_blend(int enable) {
    _stbip_alpha_blend_enabled = (enable != 0);
}

/* ---- New in v1.1.0: Antialiased line (Wu) ------------------------------ */

static void _stbip_plot_aa(stbi_uc *data, int w, int h, int c,
                           int x, int y, float brightness,
                           const stbi_uc color[4]) {
    if (brightness <= 0.0f) return;
    unsigned char col[4];
    for (int i = 0; i < 3; ++i) col[i] = (unsigned char)(color[i] * brightness + 0.5f);
    if (c == 4) {
        col[3] = (unsigned char)(color[3] * brightness + 0.5f);
    } else {
        col[3] = 255; /* unused if not blending */
    }
    _stbip_set_pixel(data, w, h, c, x, y, col);
}

STBIPDEF int stbip_draw_line_aa(stbi_uc *data, int w, int h, int c,
                                int x0, int y0, int x1, int y1,
                                const stbi_uc color[4], int thickness) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !color) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    if (thickness <= 0) thickness = 1;

    /* Wu's algorithm for antialiased lines (single-pixel width, then thicken) */
    /* We draw a thin antialiased line and then offset parallel lines for thickness */
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    /* For thickness > 1, we draw multiple thin lines with offset perpendicular to direction */
    /* Simple approach: for each offset in [-thickness/2, thickness/2], draw thin line */
    int half = thickness / 2;
    for (int off = -half; off <= thickness - half - 1; ++off) {
        int nx0 = x0, ny0 = y0, nx1 = x1, ny1 = y1;
        /* Offset perpendicular: swap dx/dy and normalize direction */
        if (dx > dy) {
            int perp = off;
            /* approximate perpendicular offset by moving in y direction */
            /* This is a crude approximation; for better results we'd compute normalized normal */
            /* Simple: offset y by off, but ensure it's within bounds */
            ny0 += off; ny1 += off;
        } else {
            nx0 += off; nx1 += off;
        }
        /* Clip offsets to keep line inside image? Not necessary; _stbip_set_pixel clips */
        /* Draw thin antialiased line using Wu algorithm */
        int edx = x1 - x0;
        int edy = y1 - y0;
        float grad;
        if (edx == 0) grad = 1.0f;
        else if (edy == 0) grad = 0.0f;
        else grad = (float)edy / edx;
        float intery = y0 + grad * 0.5f; /* starting point */
        int x_start, x_end, step;
        if (edx >= 0) { x_start = x0; x_end = x1; step = 1; }
        else { x_start = x1; x_end = x0; step = -1; }
        int y = y0;
        for (int x = x_start; x != x_end + step; x += step) {
            float bright1 = 1.0f - (intery - floorf(intery));
            float bright2 = intery - floorf(intery);
            _stbip_plot_aa(data, w, h, c, x, (int)intery, bright1, color);
            _stbip_plot_aa(data, w, h, c, x, (int)intery + 1, bright2, color);
            intery += grad;
        }
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- New in v1.1.0: Cubic Bezier curve -------------------------------- */

STBIPDEF int stbip_draw_bezier(stbi_uc *data, int w, int h, int c,
                               int x0, int y0, int x1, int y1,
                               int x2, int y2, int x3, int y3,
                               const stbi_uc color[4], int thickness) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !color) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    if (thickness <= 0) thickness = 1;

    /* Use de Casteljau subdivision or fixed sampling */
    int segments = 50; /* more segments for smoother curve */
    float t_prev = 0.0f;
    int prev_x = x0, prev_y = y0;
    for (int i = 1; i <= segments; ++i) {
        float t = (float)i / segments;
        /* Bernstein polynomial */
        float u = 1.0f - t;
        float b0 = u*u*u;
        float b1 = 3.0f*t*u*u;
        float b2 = 3.0f*t*t*u;
        float b3 = t*t*t;
        int cur_x = (int)(b0*x0 + b1*x1 + b2*x2 + b3*x3 + 0.5f);
        int cur_y = (int)(b0*y0 + b1*y1 + b2*y2 + b3*y3 + 0.5f);
        /* Draw line segment using antialiased line */
        stbip_draw_line_aa(data, w, h, c, prev_x, prev_y, cur_x, cur_y, color, thickness);
        prev_x = cur_x;
        prev_y = cur_y;
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

/* ---- TrueType support (optional) ---------------------------------------- */

#ifdef STBIP_USE_TRUETYPE

# ifndef STBIP_NOT_AUTO_INCLUDE_TRUETYPE
#  define STB_TRUETYPE_IMPLEMENTATION
#  include "stb_truetype.h"
# endif

#ifndef STBIP_MAX_TTF_FONTS
#define STBIP_MAX_TTF_FONTS 4
#endif

typedef struct {
    unsigned char *data;
    size_t size;
    stbtt_fontinfo info;
    int loaded;   /* 1 if this slot is valid */
} _stbip_ttf_font;

static _stbip_ttf_font g_ttf_fonts[STBIP_MAX_TTF_FONTS];
static int g_ttf_current = -1; /* index of current font, -1 if none */

/* helper to find a free slot */
static int _stbip_ttf_find_free_slot(void) {
    for (int i = 0; i < STBIP_MAX_TTF_FONTS; ++i) {
        if (!g_ttf_fonts[i].loaded)
            return i;
    }
    return -1;
}

/* internal function to read file into memory */
static unsigned char* _stbip_ttf_read_file(const char *path, size_t *out_size) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size <= 0) {
        fclose(f);
        return NULL;
    }
    unsigned char *buf = (unsigned char*)malloc(size);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    size_t read = fread(buf, 1, size, f);
    fclose(f);
    if (read != (size_t)size) {
        free(buf);
        return NULL;
    }
    *out_size = (size_t)size;
    return buf;
}

STBIPDEF int stbip_ttf_load(const char *path) {
    if (!path) {
        _stbip_set_error(STBIP_ERROR_INVALID_PARAM);
        return -1;
    }
    int slot = _stbip_ttf_find_free_slot();
    if (slot < 0) {
        _stbip_set_error(STBIP_ERROR_TOO_MANY_FONTS);
        return -1;
    }
    size_t size;
    unsigned char *data = _stbip_ttf_read_file(path, &size);
    if (!data) {
        _stbip_set_error(STBIP_ERROR_FILE_IO);
        return -1;
    }
    if (!stbtt_InitFont(&g_ttf_fonts[slot].info, data, 0)) {
        free(data);
        _stbip_set_error(STBIP_ERROR_UNSUPPORTED); /* font init failed */
        return -1;
    }
    g_ttf_fonts[slot].data = data;
    g_ttf_fonts[slot].size = size;
    g_ttf_fonts[slot].loaded = 1;
    /* automatically set as current if no current font */
    if (g_ttf_current < 0) {
        g_ttf_current = slot;
    }
    _stbip_set_error(STBIP_ERROR_NONE);
    return slot;
}

STBIPDEF int stbip_ttf_set_current(int font_index) {
    if (font_index < 0 || font_index >= STBIP_MAX_TTF_FONTS || !g_ttf_fonts[font_index].loaded) {
        _stbip_set_error(STBIP_ERROR_INVALID_PARAM);
        return 0;
    }
    g_ttf_current = font_index;
    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

STBIPDEF void stbip_ttf_done(void) {
    for (int i = 0; i < STBIP_MAX_TTF_FONTS; ++i) {
        if (g_ttf_fonts[i].loaded) {
            if (g_ttf_fonts[i].data) {
                free(g_ttf_fonts[i].data);
                g_ttf_fonts[i].data = NULL;
            }
            g_ttf_fonts[i].loaded = 0;
        }
    }
    g_ttf_current = -1;
    _stbip_set_error(STBIP_ERROR_NONE);
}

/* Core rendering function for TrueType text (used by both UTF?8 and ANSI versions) */
static int _stbip_draw_text_ttf_core(stbi_uc *data, int w, int h, int c,
                                     int x, int y, const char *text,
                                     const stbi_uc color[4], float font_size,
                                     int (*decode)(const char **)) {
    if (!data || w <= 0 || h <= 0 || c < 1 || c > 4 || !text || !color || font_size <= 0) {
        _stbip_set_error(STBIP_ERROR_INVALID_IMAGE);
        return 0;
    }
    if (g_ttf_current < 0 || !g_ttf_fonts[g_ttf_current].loaded) {
        _stbip_set_error(STBIP_ERROR_FONT_NOT_LOADED);
        return 0;
    }
    stbtt_fontinfo *font = &g_ttf_fonts[g_ttf_current].info;
    float scale = stbtt_ScaleForPixelHeight(font, font_size);

    int cursor_x = x;
    int cursor_y = y;

    const char *p = text;
    while (*p) {
        int codepoint = decode(&p);
        if (codepoint < 32) continue; /* skip control characters except space (32) */
        if (codepoint == 0xFFFD) continue; /* replacement char, skip */

        int advance, lsb;
        stbtt_GetCodepointHMetrics(font, codepoint, &advance, &lsb);
        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(font, codepoint, scale, scale, &x0, &y0, &x1, &y1);

        int glyph_w = x1 - x0;
        int glyph_h = y1 - y0;
        if (glyph_w > 0 && glyph_h > 0) {
            unsigned char *bitmap = (unsigned char*)malloc(glyph_w * glyph_h);
            if (!bitmap) {
                _stbip_set_error(STBIP_ERROR_OUT_OF_MEMORY);
                return 0;
            }
            stbtt_MakeCodepointBitmap(font, bitmap, glyph_w, glyph_h, glyph_w, scale, scale, codepoint);

            int draw_x = cursor_x + x0;
            int draw_y = cursor_y - y1;  /* stb_truetype y is inverted */
            for (int row = 0; row < glyph_h; ++row) {
                int py = draw_y + row;
                if (py < 0 || py >= h) continue;
                for (int col = 0; col < glyph_w; ++col) {
                    unsigned char alpha = bitmap[row * glyph_w + col];
                    if (alpha > 0) {
                        int px = draw_x + col;
                        /* Use alpha as brightness for the pixel; set color alpha accordingly */
                        unsigned char col_with_alpha[4];
                        for (int i = 0; i < 3; ++i)
                            col_with_alpha[i] = (unsigned char)(color[i] * (alpha / 255.0f) + 0.5f);
                        if (c == 4)
                            col_with_alpha[3] = (unsigned char)(color[3] * (alpha / 255.0f) + 0.5f);
                        else
                            col_with_alpha[3] = 255;
                        _stbip_set_pixel(data, w, h, c, px, py, col_with_alpha);
                    }
                }
            }
            free(bitmap);
        }

        cursor_x += (int)(advance * scale);
        if (cursor_x >= w) break;
    }

    _stbip_set_error(STBIP_ERROR_NONE);
    return 1;
}

STBIPDEF int stbip_draw_text_ttf_utf8(stbi_uc *data, int w, int h, int c,
                                      int x, int y, const char *text,
                                      const stbi_uc color[4], float font_size) {
    return _stbip_draw_text_ttf_core(data, w, h, c, x, y, text, color, font_size, _stbip_utf8_decode);
}

STBIPDEF int stbip_draw_text_ttf_ansi(stbi_uc *data, int w, int h, int c,
                                      int x, int y, const char *text,
                                      const stbi_uc color[4], float font_size) {
    if (!text) {
        _stbip_set_error(STBIP_ERROR_INVALID_PARAM);
        return 0;
    }
    /* Convert ANSI string to wide characters using current locale */
    size_t len = mbstowcs(NULL, text, 0);
    if (len == (size_t)-1) {
        _stbip_set_error(STBIP_ERROR_ENCODING);
        return 0;
    }
    wchar_t *wstr = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
    if (!wstr) {
        _stbip_set_error(STBIP_ERROR_OUT_OF_MEMORY);
        return 0;
    }
    mbstowcs(wstr, text, len + 1);
    wstr[len] = L'\0';

    /* Convert wide string to UTF?8 */
    size_t utf8_len = 0;
    for (size_t i = 0; i < len; ++i) {
        unsigned int cp = (unsigned int)wstr[i];
        if (cp < 0x80) utf8_len += 1;
        else if (cp < 0x800) utf8_len += 2;
        else if (cp < 0x10000) utf8_len += 3;
        else if (cp < 0x110000) utf8_len += 4;
        else utf8_len += 1; /* invalid, will be replaced by '?' */
    }
    char *utf8 = (char*)malloc(utf8_len + 1);
    if (!utf8) {
        free(wstr);
        _stbip_set_error(STBIP_ERROR_OUT_OF_MEMORY);
        return 0;
    }
    size_t pos = 0;
    for (size_t i = 0; i < len; ++i) {
        unsigned int cp = (unsigned int)wstr[i];
        if (cp < 0x80) {
            utf8[pos++] = (char)cp;
        } else if (cp < 0x800) {
            utf8[pos++] = (char)(0xC0 | (cp >> 6));
            utf8[pos++] = (char)(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            utf8[pos++] = (char)(0xE0 | (cp >> 12));
            utf8[pos++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            utf8[pos++] = (char)(0x80 | (cp & 0x3F));
        } else if (cp < 0x110000) {
            utf8[pos++] = (char)(0xF0 | (cp >> 18));
            utf8[pos++] = (char)(0x80 | ((cp >> 12) & 0x3F));
            utf8[pos++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            utf8[pos++] = (char)(0x80 | (cp & 0x3F));
        } else {
            utf8[pos++] = '?'; /* replacement for invalid */
        }
    }
    utf8[pos] = '\0';
    free(wstr);

    /* Now render the UTF?8 string by calling the UTF?8 version */
    int ret = stbip_draw_text_ttf_utf8(data, w, h, c, x, y, utf8, color, font_size);
    free(utf8);
    return ret;
}

#endif /* STBIP_USE_TRUETYPE */

#endif /* STB_IMAGE_PAINT_IMPLEMENTATION */

#endif /* STBIP_IMAGE_PAINT_H_INCLUDED */
