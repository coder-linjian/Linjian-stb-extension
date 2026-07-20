/* stb_image_edit.h - v1.0.0 - image processing extensions for stb_image
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
 *   Define STB_IMAGE_EDIT_IMPLEMENT in exactly ONE C file before including
 *   this header to generate the implementation.
 *
 *   #define STB_IMAGE_EDIT_IMPLEMENT
 *   #include "stb_image_edit.h"
 *
 *   All functions operate on 8?bit unsigned interleaved pixel data.
 *   The caller is responsible for allocating output buffers of sufficient size.
 *
 * ============================================================================
 */

#ifndef STBIE_EDIT_H_INCLUDED
#define STBIE_EDIT_H_INCLUDED

#ifndef STBIE_EDIT_VERSION
#define STBIE_EDIT_VERSION 100   // v1.0.0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ----------- API linkage macro ----------- */
#ifndef STBIEDEF
#define STBIEDEF static   // override to extern or dllimport/dllexport if needed
#endif

/* ----------- basic types ----------- */
typedef unsigned char stbi_uc;

/* ----------- error codes ----------- */
typedef enum {
    STBIE_ERROR_NONE = 0,
    STBIE_ERROR_INVALID_PARAM,      // NULL pointer or non-positive dimensions
    STBIE_ERROR_UNSUPPORTED_CODE,   // conversion code not supported
    STBIE_ERROR_CHANNEL_MISMATCH,   // input/output channel count mismatch
    STBIE_ERROR_NOT_IMPLEMENTED,    // function called but not yet implemented
    STBIE_ERROR_LAST                // keep last
} stbie_error;

/* ----------- color conversion codes ----------- */
#define STBIE_COLOR_RGB2GRAY    1000
#define STBIE_COLOR_GRAY2RGB    1001
#define STBIE_COLOR_RGB2YUV     1002
#define STBIE_COLOR_YUV2RGB     1003
#define STBIE_COLOR_RGB2HSV     1004
#define STBIE_COLOR_HSV2RGB     1005
#define STBIE_COLOR_RGB2BGR     1006   // channel swap
#define STBIE_COLOR_BGR2RGB     1007

/* ----------- border handling modes ----------- */
#define STBIE_BORDER_REPLICATE  2000
#define STBIE_BORDER_REFLECT    2001
#define STBIE_BORDER_ZERO       2002

/* ----------- rotation modes ----------- */
#define STBIE_ROTATE_CROP       0
#define STBIE_ROTATE_EXPAND     1

/* ----------- main API functions ----------- */

/* ----- error handling ----- */
STBIEDEF stbie_error stbie_get_last_error(void);
STBIEDEF const char* stbie_get_error_string(stbie_error err);

/* ----- color space conversion ----- */
STBIEDEF int stbie_cvt_color(const stbi_uc *input, int w, int h, int channels_in,
                             stbi_uc *output, int channels_out, int code);

/* ----- filtering & convolution ----- */
STBIEDEF int stbie_filter_2d(const stbi_uc *input, int w, int h, int channels,
                             stbi_uc *output,
                             const float *kernel, int k_w, int k_h,
                             int border_mode);

/* ----- Sobel edge detection ----- */
STBIEDEF int stbie_sobel(const stbi_uc *input, int w, int h, int channels,
                         stbi_uc *output, int border_mode);

/* ----- image rotation ----- */
STBIEDEF void stbie_rotate_calc_size(int w, int h, float angle_deg,
                                     int *out_w, int *out_h);
STBIEDEF int stbie_rotate(const stbi_uc *input, int w, int h, int channels,
                          stbi_uc *output, int *out_w, int *out_h,
                          float angle_deg, int mode);

/* ----- image cropping ----- */
STBIEDEF int stbie_crop(const stbi_uc *input, int w, int h, int channels,
                        int crop_x, int crop_y, int crop_w, int crop_h,
                        stbi_uc *output);

/* ----- Gaussian blur ----- */
STBIEDEF int stbie_blur_gaussian(const stbi_uc *input, int w, int h, int channels,
                                 stbi_uc *output, float sigma, int ksize,
                                 int border_mode);

/* ----- Median blur ----- */
STBIEDEF int stbie_median_blur(const stbi_uc *input, int w, int h, int channels,
                               stbi_uc *output, int ksize, int border_mode);

/* ----- Mean blur (box filter) ----- */
STBIEDEF int stbie_blur(const stbi_uc *input, int w, int h, int channels,
                        stbi_uc *output, int ksize, int border_mode);

/* ----- Canny edge detection ----- */
STBIEDEF int stbie_canny(const stbi_uc *input, int w, int h, int channels,
                         stbi_uc *output,
                         int low_thresh, int high_thresh,
                         float sigma, int gaussian_ksize,
                         int border_mode);

/* ----- pixel access utilities (inline) ----- */
static inline int stbie_get_pixel(const stbi_uc *data, int w, int h, int channels,
                                  int x, int y, stbi_uc *out)
{
    if (!data || !out || channels <= 0) return 0;
    if (x < 0) x = 0;
    else if (x >= w) x = w - 1;
    if (y < 0) y = 0;
    else if (y >= h) y = h - 1;
    const stbi_uc *p = data + (y * w + x) * channels;
    for (int c = 0; c < channels; ++c) out[c] = p[c];
    return 1;
}

static inline int stbie_set_pixel(stbi_uc *data, int w, int h, int channels,
                                  int x, int y, const stbi_uc *in)
{
    if (!data || !in || channels <= 0) return 0;
    if (x < 0 || x >= w || y < 0 || y >= h) return 0;
    stbi_uc *p = data + (y * w + x) * channels;
    for (int c = 0; c < channels; ++c) p[c] = in[c];
    return 1;
}

/* ----------- utility macros ----------- */
#ifndef STBIE_ASSERT
#include <assert.h>
#define STBIE_ASSERT(x) assert(x)
#endif

#ifdef __cplusplus
}
#endif

/* ============================================================================
   IMPLEMENTATION
   ============================================================================ */
#ifdef STB_IMAGE_EDIT_IMPLEMENT

#ifndef STB_IMAGE_EDIT_IMPLEMENT_ONCE
#define STB_IMAGE_EDIT_IMPLEMENT_ONCE

#include <math.h>   // for sqrtf, sinf, cosf, expf, atan2f
#include <string.h> // for memcpy

/* -------- internal error handling -------- */
static stbie_error _stbie_last_error = STBIE_ERROR_NONE;
#define _STBIE_SET_ERROR(err) do { _stbie_last_error = (err); } while(0)

/* -------- internal helpers -------- */
static stbi_uc _stbie_clamp_uc(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (stbi_uc)v;
}

/* ---------- border-aware pixel access ---------- */
static stbi_uc _stbie_get_pixel_border(const stbi_uc *data, int w, int h, int channels,
                                       int x, int y, int c, int border_mode)
{
    switch (border_mode) {
        case STBIE_BORDER_REPLICATE:
            if (x < 0) x = 0;
            else if (x >= w) x = w - 1;
            if (y < 0) y = 0;
            else if (y >= h) y = h - 1;
            break;
        case STBIE_BORDER_REFLECT:
            while (x < 0) x = -x - 1;
            while (x >= w) x = 2 * w - x - 1;
            while (y < 0) y = -y - 1;
            while (y >= h) y = 2 * h - y - 1;
            break;
        case STBIE_BORDER_ZERO:
            if (x < 0 || x >= w || y < 0 || y >= h)
                return 0;
            break;
        default:
            return 0;
    }
    return data[(y * w + x) * channels + c];
}

/* -------- color conversion routines -------- */
static stbi_uc _stbie_rgb2gray_int(const stbi_uc *rgb) {
    int y = (rgb[0] * 19595 + rgb[1] * 38469 + rgb[2] * 7472 + 32768) >> 16;
    return _stbie_clamp_uc(y);
}

static void _stbie_rgb2yuv_int(const stbi_uc *src, stbi_uc *dst) {
    int r = src[0], g = src[1], b = src[2];
    int y  = (r * 19595 + g * 38469 + b * 7472  + 32768) >> 16;
    int u  = (r * -9633 + g * -18920 + b * 28565 + 32768) >> 16;
    int v  = (r * 40297 + g * -33751 + b * -6554  + 32768) >> 16;
    dst[0] = _stbie_clamp_uc(y);
    dst[1] = _stbie_clamp_uc(u + 128);
    dst[2] = _stbie_clamp_uc(v + 128);
}

static void _stbie_yuv2rgb_int(const stbi_uc *src, stbi_uc *dst) {
    int y = src[0], u = src[1] - 128, v = src[2] - 128;
    int r = y + ((v * 91880 + 32768) >> 16);
    int g = y - ((u * 22536 + 32768) >> 16) - ((v * 46798 + 32768) >> 16);
    int b = y + ((u * 116130 + 32768) >> 16);
    dst[0] = _stbie_clamp_uc(r);
    dst[1] = _stbie_clamp_uc(g);
    dst[2] = _stbie_clamp_uc(b);
}

static void _stbie_rgb2hsv_float(const stbi_uc *src, stbi_uc *dst) {
    float r = src[0]/255.0f, g = src[1]/255.0f, b = src[2]/255.0f;
    float max = r, min = r;
    if (g > max) max = g; if (b > max) max = b;
    if (g < min) min = g; if (b < min) min = b;
    float delta = max - min;
    float h = 0.0f, s = 0.0f, v = max;
    if (delta != 0.0f) {
        s = delta / max;
        if (max == r) h = (g - b) / delta + (g < b ? 6.0f : 0.0f);
        else if (max == g) h = (b - r) / delta + 2.0f;
        else h = (r - g) / delta + 4.0f;
        h *= 60.0f;
    }
    dst[0] = _stbie_clamp_uc((int)(h / 360.0f * 255.0f + 0.5f));
    dst[1] = _stbie_clamp_uc((int)(s * 255.0f + 0.5f));
    dst[2] = _stbie_clamp_uc((int)(v * 255.0f + 0.5f));
}

static void _stbie_hsv2rgb_float(const stbi_uc *src, stbi_uc *dst) {
    float h = src[0] / 255.0f * 360.0f;
    float s = src[1] / 255.0f;
    float v = src[2] / 255.0f;
    float r, g, b;
    int i = (int)(h / 60.0f) % 6;
    float f = h / 60.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }
    dst[0] = _stbie_clamp_uc((int)(r * 255.0f + 0.5f));
    dst[1] = _stbie_clamp_uc((int)(g * 255.0f + 0.5f));
    dst[2] = _stbie_clamp_uc((int)(b * 255.0f + 0.5f));
}

/* ----- error handling ----- */
STBIEDEF stbie_error stbie_get_last_error(void) {
    return _stbie_last_error;
}

STBIEDEF const char* stbie_get_error_string(stbie_error err) {
    switch (err) {
        case STBIE_ERROR_NONE:              return "No error";
        case STBIE_ERROR_INVALID_PARAM:     return "Invalid parameter (NULL pointer or non-positive dimensions)";
        case STBIE_ERROR_UNSUPPORTED_CODE:  return "Unsupported conversion code";
        case STBIE_ERROR_CHANNEL_MISMATCH:  return "Input/output channel count mismatch";
        case STBIE_ERROR_NOT_IMPLEMENTED:   return "Function not yet implemented";
        default:                            return "Unknown error";
    }
}

/* ----- color conversion dispatcher ----- */
STBIEDEF int stbie_cvt_color(const stbi_uc *input, int w, int h, int channels_in,
                             stbi_uc *output, int channels_out, int code)
{
    if (!input || !output || w <= 0 || h <= 0) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    int total_pixels = w * h;
    switch (code) {
        case STBIE_COLOR_RGB2GRAY: {
            if (channels_in != 3 || channels_out != 1) {
                _STBIE_SET_ERROR(STBIE_ERROR_CHANNEL_MISMATCH);
                return 0;
            }
            for (int i = 0; i < total_pixels; ++i)
                output[i] = _stbie_rgb2gray_int(input + i * 3);
            _STBIE_SET_ERROR(STBIE_ERROR_NONE);
            return 1;
        }
        case STBIE_COLOR_GRAY2RGB: {
            if (channels_in != 1 || channels_out != 3) {
                _STBIE_SET_ERROR(STBIE_ERROR_CHANNEL_MISMATCH);
                return 0;
            }
            for (int i = 0; i < total_pixels; ++i) {
                stbi_uc val = input[i];
                output[i*3] = output[i*3+1] = output[i*3+2] = val;
            }
            _STBIE_SET_ERROR(STBIE_ERROR_NONE);
            return 1;
        }
        case STBIE_COLOR_RGB2YUV: {
            if (channels_in != 3 || channels_out != 3) {
                _STBIE_SET_ERROR(STBIE_ERROR_CHANNEL_MISMATCH);
                return 0;
            }
            for (int i = 0; i < total_pixels; ++i)
                _stbie_rgb2yuv_int(input + i*3, output + i*3);
            _STBIE_SET_ERROR(STBIE_ERROR_NONE);
            return 1;
        }
        case STBIE_COLOR_YUV2RGB: {
            if (channels_in != 3 || channels_out != 3) {
                _STBIE_SET_ERROR(STBIE_ERROR_CHANNEL_MISMATCH);
                return 0;
            }
            for (int i = 0; i < total_pixels; ++i)
                _stbie_yuv2rgb_int(input + i*3, output + i*3);
            _STBIE_SET_ERROR(STBIE_ERROR_NONE);
            return 1;
        }
        case STBIE_COLOR_RGB2HSV: {
            if (channels_in != 3 || channels_out != 3) {
                _STBIE_SET_ERROR(STBIE_ERROR_CHANNEL_MISMATCH);
                return 0;
            }
            for (int i = 0; i < total_pixels; ++i)
                _stbie_rgb2hsv_float(input + i*3, output + i*3);
            _STBIE_SET_ERROR(STBIE_ERROR_NONE);
            return 1;
        }
        case STBIE_COLOR_HSV2RGB: {
            if (channels_in != 3 || channels_out != 3) {
                _STBIE_SET_ERROR(STBIE_ERROR_CHANNEL_MISMATCH);
                return 0;
            }
            for (int i = 0; i < total_pixels; ++i)
                _stbie_hsv2rgb_float(input + i*3, output + i*3);
            _STBIE_SET_ERROR(STBIE_ERROR_NONE);
            return 1;
        }
        case STBIE_COLOR_RGB2BGR:
        case STBIE_COLOR_BGR2RGB: {
            if (channels_in != 3 || channels_out != 3) {
                _STBIE_SET_ERROR(STBIE_ERROR_CHANNEL_MISMATCH);
                return 0;
            }
            for (int i = 0; i < total_pixels; ++i) {
                const stbi_uc *p = input + i*3;
                stbi_uc *q = output + i*3;
                q[0] = p[2]; q[1] = p[1]; q[2] = p[0];
            }
            _STBIE_SET_ERROR(STBIE_ERROR_NONE);
            return 1;
        }
        default: {
            _STBIE_SET_ERROR(STBIE_ERROR_UNSUPPORTED_CODE);
            return 0;
        }
    }
}

/* ----- stbie_filter_2d ----- */
STBIEDEF int stbie_filter_2d(const stbi_uc *input, int w, int h, int channels,
                             stbi_uc *output,
                             const float *kernel, int k_w, int k_h,
                             int border_mode)
{
    if (!input || !output || !kernel || w <= 0 || h <= 0 || channels <= 0 ||
        k_w <= 0 || k_h <= 0) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (border_mode != STBIE_BORDER_REPLICATE &&
        border_mode != STBIE_BORDER_REFLECT &&
        border_mode != STBIE_BORDER_ZERO) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    int half_kw = k_w / 2;
    int half_kh = k_h / 2;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0f;
                for (int ky = -half_kh; ky <= half_kh; ++ky) {
                    for (int kx = -half_kw; kx <= half_kw; ++kx) {
                        int ix = x + kx;
                        int iy = y + ky;
                        stbi_uc pixel = _stbie_get_pixel_border(input, w, h, channels,
                                                                 ix, iy, c, border_mode);
                        float k = kernel[(ky + half_kh) * k_w + (kx + half_kw)];
                        sum += k * pixel;
                    }
                }
                int val = (int)(sum + 0.5f);
                output[(y * w + x) * channels + c] = _stbie_clamp_uc(val);
            }
        }
    }
    _STBIE_SET_ERROR(STBIE_ERROR_NONE);
    return 1;
}

/* ----- stbie_sobel ----- */
STBIEDEF int stbie_sobel(const stbi_uc *input, int w, int h, int channels,
                         stbi_uc *output, int border_mode)
{
    if (!input || !output || w <= 0 || h <= 0) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (border_mode != STBIE_BORDER_REPLICATE &&
        border_mode != STBIE_BORDER_REFLECT &&
        border_mode != STBIE_BORDER_ZERO) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    const stbi_uc *gray_input = input;
    stbi_uc *gray_temp = NULL;
    if (channels != 1) {
        if (channels != 3) {
            _STBIE_SET_ERROR(STBIE_ERROR_CHANNEL_MISMATCH);
            return 0;
        }
        gray_temp = (stbi_uc*)malloc(w * h);
        if (!gray_temp) {
            _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
            return 0;
        }
        if (!stbie_cvt_color(input, w, h, 3, gray_temp, 1, STBIE_COLOR_RGB2GRAY)) {
            free(gray_temp);
            return 0;
        }
        gray_input = gray_temp;
    }

    float sobel_x[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    float sobel_y[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};

    stbi_uc *gx = (stbi_uc*)malloc(w * h);
    stbi_uc *gy = (stbi_uc*)malloc(w * h);
    if (!gx || !gy) {
        free(gx);
        free(gy);
        if (gray_temp) free(gray_temp);
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    int success = 1;
    if (!stbie_filter_2d(gray_input, w, h, 1, gx, &sobel_x[0][0], 3, 3, border_mode)) {
        success = 0;
        goto cleanup;
    }
    if (!stbie_filter_2d(gray_input, w, h, 1, gy, &sobel_y[0][0], 3, 3, border_mode)) {
        success = 0;
        goto cleanup;
    }

    for (int i = 0; i < w * h; ++i) {
        float fx = gx[i];
        float fy = gy[i];
        float mag = sqrtf(fx * fx + fy * fy);
        int val = (int)(mag + 0.5f);
        output[i] = _stbie_clamp_uc(val);
    }

    _STBIE_SET_ERROR(STBIE_ERROR_NONE);

cleanup:
    free(gx);
    free(gy);
    if (gray_temp) free(gray_temp);
    return success;
}

/* ----- stbie_rotate_calc_size ----- */
STBIEDEF void stbie_rotate_calc_size(int w, int h, float angle_deg,
                                     int *out_w, int *out_h)
{
    float rad = angle_deg * 3.141592653589793f / 180.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);

    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float pts[4][2] = {
        {-cx, -cy},
        { w - cx, -cy},
        { w - cx,  h - cy},
        {-cx,  h - cy}
    };

    float min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    for (int i = 0; i < 4; ++i) {
        float x = pts[i][0] * cos_a - pts[i][1] * sin_a;
        float y = pts[i][0] * sin_a + pts[i][1] * cos_a;
        if (i == 0) {
            min_x = max_x = x;
            min_y = max_y = y;
        } else {
            if (x < min_x) min_x = x;
            if (x > max_x) max_x = x;
            if (y < min_y) min_y = y;
            if (y > max_y) max_y = y;
        }
    }
    *out_w = (int)(max_x - min_x + 0.5f);
    *out_h = (int)(max_y - min_y + 0.5f);
}

/* ----- stbie_rotate ----- */
STBIEDEF int stbie_rotate(const stbi_uc *input, int w, int h, int channels,
                          stbi_uc *output, int *out_w, int *out_h,
                          float angle_deg, int mode)
{
    if (!input || !output || w <= 0 || h <= 0 || channels <= 0) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (mode != STBIE_ROTATE_CROP && mode != STBIE_ROTATE_EXPAND) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    int out_width, out_height;
    if (mode == STBIE_ROTATE_CROP) {
        out_width = w;
        out_height = h;
        if (out_w) *out_w = w;
        if (out_h) *out_h = h;
    } else {
        stbie_rotate_calc_size(w, h, angle_deg, &out_width, &out_height);
        if (out_w) *out_w = out_width;
        if (out_h) *out_h = out_height;
    }

    float rad = angle_deg * 3.141592653589793f / 180.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);

    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float cx_out = out_width / 2.0f;
    float cy_out = out_height / 2.0f;

    int stride = out_width * channels;

    for (int y_out = 0; y_out < out_height; ++y_out) {
        for (int x_out = 0; x_out < out_width; ++x_out) {
            float x_shift = x_out - cx_out;
            float y_shift = y_out - cy_out;
            float src_x = x_shift * cos_a + y_shift * sin_a + cx;
            float src_y = -x_shift * sin_a + y_shift * cos_a + cy;

            for (int c = 0; c < channels; ++c) {
                float val;
                if (src_x < 0 || src_x >= w || src_y < 0 || src_y >= h) {
                    val = 0.0f;
                } else {
                    int x0 = (int)src_x;
                    int y0 = (int)src_y;
                    int x1 = (x0 + 1 < w) ? x0 + 1 : x0;
                    int y1 = (y0 + 1 < h) ? y0 + 1 : y0;
                    float fx = src_x - x0;
                    float fy = src_y - y0;

                    float v00 = input[(y0 * w + x0) * channels + c];
                    float v10 = input[(y0 * w + x1) * channels + c];
                    float v01 = input[(y1 * w + x0) * channels + c];
                    float v11 = input[(y1 * w + x1) * channels + c];

                    float v0 = v00 * (1 - fx) + v10 * fx;
                    float v1 = v01 * (1 - fx) + v11 * fx;
                    val = v0 * (1 - fy) + v1 * fy;
                }
                int int_val = (int)(val + 0.5f);
                output[y_out * stride + x_out * channels + c] = _stbie_clamp_uc(int_val);
            }
        }
    }

    _STBIE_SET_ERROR(STBIE_ERROR_NONE);
    return 1;
}

/* ----- stbie_crop ----- */
STBIEDEF int stbie_crop(const stbi_uc *input, int w, int h, int channels,
                        int crop_x, int crop_y, int crop_w, int crop_h,
                        stbi_uc *output)
{
    if (!input || !output || w <= 0 || h <= 0 || channels <= 0 ||
        crop_w <= 0 || crop_h <= 0 || crop_x < 0 || crop_y < 0 ||
        crop_x + crop_w > w || crop_y + crop_h > h) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    int row_bytes = crop_w * channels;
    for (int y = 0; y < crop_h; ++y) {
        const stbi_uc *src_row = input + ((crop_y + y) * w + crop_x) * channels;
        stbi_uc *dst_row = output + y * row_bytes;
        memcpy(dst_row, src_row, row_bytes);
    }
    _STBIE_SET_ERROR(STBIE_ERROR_NONE);
    return 1;
}

/* ----- Gaussian blur (separable) ----- */
STBIEDEF int stbie_blur_gaussian(const stbi_uc *input, int w, int h, int channels,
                                 stbi_uc *output, float sigma, int ksize,
                                 int border_mode)
{
    if (!input || !output || w <= 0 || h <= 0 || channels <= 0 ||
        ksize <= 0 || (ksize % 2) == 0 || sigma <= 0.0f) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (border_mode != STBIE_BORDER_REPLICATE &&
        border_mode != STBIE_BORDER_REFLECT &&
        border_mode != STBIE_BORDER_ZERO) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    int half = ksize / 2;
    float *kernel = (float*)malloc(ksize * sizeof(float));
    if (!kernel) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    float sum = 0.0f;
    float sigma2 = 2.0f * sigma * sigma;
    for (int i = 0; i < ksize; ++i) {
        int x = i - half;
        kernel[i] = expf(- (x * x) / sigma2);
        sum += kernel[i];
    }
    for (int i = 0; i < ksize; ++i) kernel[i] /= sum;

    int total_pixels = w * h * channels;
    stbi_uc *temp = (stbi_uc*)malloc(total_pixels);
    if (!temp) {
        free(kernel);
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    // Horizontal pass
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < channels; ++c) {
                float acc = 0.0f;
                for (int kx = -half; kx <= half; ++kx) {
                    int ix = x + kx;
                    stbi_uc pixel = _stbie_get_pixel_border(input, w, h, channels,
                                                            ix, y, c, border_mode);
                    acc += kernel[kx + half] * pixel;
                }
                int val = (int)(acc + 0.5f);
                temp[(y * w + x) * channels + c] = _stbie_clamp_uc(val);
            }
        }
    }

    // Vertical pass
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < channels; ++c) {
                float acc = 0.0f;
                for (int ky = -half; ky <= half; ++ky) {
                    int iy = y + ky;
                    stbi_uc pixel = _stbie_get_pixel_border(temp, w, h, channels,
                                                            x, iy, c, border_mode);
                    acc += kernel[ky + half] * pixel;
                }
                int val = (int)(acc + 0.5f);
                output[(y * w + x) * channels + c] = _stbie_clamp_uc(val);
            }
        }
    }

    free(kernel);
    free(temp);
    _STBIE_SET_ERROR(STBIE_ERROR_NONE);
    return 1;
}

/* ----- Median blur ----- */
STBIEDEF int stbie_median_blur(const stbi_uc *input, int w, int h, int channels,
                               stbi_uc *output, int ksize, int border_mode)
{
    if (!input || !output || w <= 0 || h <= 0 || channels <= 0 ||
        ksize <= 0 || (ksize % 2) == 0) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (border_mode != STBIE_BORDER_REPLICATE &&
        border_mode != STBIE_BORDER_REFLECT &&
        border_mode != STBIE_BORDER_ZERO) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    int half = ksize / 2;
    int window_size = ksize * ksize;
    int *values = (int*)malloc(window_size * sizeof(int));
    if (!values) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < channels; ++c) {
                int idx = 0;
                for (int ky = -half; ky <= half; ++ky) {
                    for (int kx = -half; kx <= half; ++kx) {
                        int ix = x + kx;
                        int iy = y + ky;
                        values[idx++] = _stbie_get_pixel_border(input, w, h, channels,
                                                                ix, iy, c, border_mode);
                    }
                }
                // Insertion sort
                for (int i = 1; i < window_size; ++i) {
                    int key = values[i];
                    int j = i - 1;
                    while (j >= 0 && values[j] > key) {
                        values[j + 1] = values[j];
                        --j;
                    }
                    values[j + 1] = key;
                }
                int median = values[window_size / 2];
                output[(y * w + x) * channels + c] = (stbi_uc)median;
            }
        }
    }

    free(values);
    _STBIE_SET_ERROR(STBIE_ERROR_NONE);
    return 1;
}

/* ----- Mean blur (box filter) ----- */
STBIEDEF int stbie_blur(const stbi_uc *input, int w, int h, int channels,
                        stbi_uc *output, int ksize, int border_mode)
{
    if (!input || !output || w <= 0 || h <= 0 || channels <= 0 ||
        ksize <= 0 || (ksize & 1) == 0) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (border_mode != STBIE_BORDER_REPLICATE &&
        border_mode != STBIE_BORDER_REFLECT &&
        border_mode != STBIE_BORDER_ZERO) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    int kernel_size = ksize * ksize;
    float *kernel = (float*)malloc(kernel_size * sizeof(float));
    if (!kernel) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    float val = 1.0f / (float)kernel_size;
    for (int i = 0; i < kernel_size; ++i)
        kernel[i] = val;

    int result = stbie_filter_2d(input, w, h, channels, output,
                                 kernel, ksize, ksize, border_mode);
    free(kernel);
    return result;
}

/* ==================== CANNY EDGE DETECTION ==================== */

/* Internal: Non-maximum suppression (fixed: first param is const float*) */
static void _stbie_non_max_suppression(const float *mag, float *angle,
                                       int w, int h,
                                       stbi_uc *nms_out)
{
    const float PI = 3.141592653589793f;
    for (int y = 1; y < h-1; ++y) {
        for (int x = 1; x < w-1; ++x) {
            int idx = y * w + x;
            float a = angle[idx];
            int dir = 0;
            if (a < 0) a += PI;
            if (a >= PI) a -= PI;
            if (a < PI/8 || a >= 7*PI/8) dir = 0;
            else if (a >= PI/8 && a < 3*PI/8) dir = 1;
            else if (a >= 3*PI/8 && a < 5*PI/8) dir = 2;
            else dir = 3;

            float m = mag[idx];
            float m1 = 0, m2 = 0;
            switch (dir) {
                case 0: m1 = mag[y * w + (x-1)]; m2 = mag[y * w + (x+1)]; break;
                case 1: m1 = mag[(y-1) * w + (x-1)]; m2 = mag[(y+1) * w + (x+1)]; break;
                case 2: m1 = mag[(y-1) * w + x]; m2 = mag[(y+1) * w + x]; break;
                case 3: m1 = mag[(y-1) * w + (x+1)]; m2 = mag[(y+1) * w + (x-1)]; break;
            }
            if (m >= m1 && m >= m2)
                nms_out[idx] = (stbi_uc)m;
            else
                nms_out[idx] = 0;
        }
    }
    for (int y = 0; y < h; ++y) {
        nms_out[y * w] = 0;
        nms_out[y * w + w-1] = 0;
    }
    for (int x = 0; x < w; ++x) {
        nms_out[x] = 0;
        nms_out[(h-1) * w + x] = 0;
    }
}

/* Internal: Hysteresis thresholding (iterative stack propagation) */
static void _stbie_hysteresis_threshold(stbi_uc *nms, int w, int h,
                                        int low_thresh, int high_thresh,
                                        stbi_uc *out)
{
    // Classify: strong = 255, weak = 128, suppress = 0
    for (int i = 0; i < w*h; ++i) {
        int v = nms[i];
        if (v >= high_thresh)
            out[i] = 255;
        else if (v >= low_thresh)
            out[i] = 128;
        else
            out[i] = 0;
    }

    // Stack-based propagation
    int *stack_x = (int*)malloc(w * h * sizeof(int));
    int *stack_y = (int*)malloc(w * h * sizeof(int));
    if (!stack_x || !stack_y) {
        free(stack_x);
        free(stack_y);
        // Fallback: keep only strong pixels
        for (int i = 0; i < w*h; ++i) {
            if (out[i] == 255) continue;
            out[i] = 0;
        }
        return;
    }
    int stack_top = 0;

    // Push all strong pixels
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (out[y * w + x] == 255) {
                stack_x[stack_top] = x;
                stack_y[stack_top] = y;
                ++stack_top;
            }
        }
    }

    // Propagate
    while (stack_top > 0) {
        --stack_top;
        int x = stack_x[stack_top];
        int y = stack_y[stack_top];
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                    int idx = ny * w + nx;
                    if (out[idx] == 128) {
                        out[idx] = 255;
                        stack_x[stack_top] = nx;
                        stack_y[stack_top] = ny;
                        ++stack_top;
                    }
                }
            }
        }
    }

    // Suppress remaining weak pixels
    for (int i = 0; i < w*h; ++i) {
        if (out[i] != 255)
            out[i] = 0;
    }

    free(stack_x);
    free(stack_y);
}

/* ----- stbie_canny (main) ----- */
STBIEDEF int stbie_canny(const stbi_uc *input, int w, int h, int channels,
                         stbi_uc *output,
                         int low_thresh, int high_thresh,
                         float sigma, int gaussian_ksize,
                         int border_mode)
{
    if (!input || !output || w <= 0 || h <= 0) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (channels != 1 && channels != 3) {
        _STBIE_SET_ERROR(STBIE_ERROR_CHANNEL_MISMATCH);
        return 0;
    }
    if (low_thresh < 0 || low_thresh > 255 || high_thresh < 0 || high_thresh > 255 ||
        low_thresh > high_thresh) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (sigma <= 0 || gaussian_ksize <= 0 || (gaussian_ksize % 2) == 0) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (border_mode != STBIE_BORDER_REPLICATE &&
        border_mode != STBIE_BORDER_REFLECT &&
        border_mode != STBIE_BORDER_ZERO) {
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }

    int total_pixels = w * h;

    // 1. Grayscale conversion if needed
    const stbi_uc *gray = input;
    stbi_uc *gray_temp = NULL;
    if (channels != 1) {
        gray_temp = (stbi_uc*)malloc(total_pixels);
        if (!gray_temp) {
            _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
            return 0;
        }
        if (!stbie_cvt_color(input, w, h, 3, gray_temp, 1, STBIE_COLOR_RGB2GRAY)) {
            free(gray_temp);
            return 0;
        }
        gray = gray_temp;
    }

    // 2. Gaussian blur
    stbi_uc *blurred = (stbi_uc*)malloc(total_pixels);
    if (!blurred) {
        if (gray_temp) free(gray_temp);
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (!stbie_blur_gaussian(gray, w, h, 1, blurred, sigma, gaussian_ksize, border_mode)) {
        free(blurred);
        if (gray_temp) free(gray_temp);
        return 0;
    }

    // 3. Sobel gradients
    float sobel_x[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    float sobel_y[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};

    stbi_uc *gx = (stbi_uc*)malloc(total_pixels);
    stbi_uc *gy = (stbi_uc*)malloc(total_pixels);
    if (!gx || !gy) {
        free(gx); free(gy);
        free(blurred);
        if (gray_temp) free(gray_temp);
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    if (!stbie_filter_2d(blurred, w, h, 1, gx, &sobel_x[0][0], 3, 3, border_mode) ||
        !stbie_filter_2d(blurred, w, h, 1, gy, &sobel_y[0][0], 3, 3, border_mode)) {
        free(gx); free(gy);
        free(blurred);
        if (gray_temp) free(gray_temp);
        return 0;
    }

    // 4. Compute magnitude and direction
    float *magnitude = (float*)malloc(total_pixels * sizeof(float));
    float *angle = (float*)malloc(total_pixels * sizeof(float));
    if (!magnitude || !angle) {
        free(magnitude); free(angle);
        free(gx); free(gy);
        free(blurred);
        if (gray_temp) free(gray_temp);
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    for (int i = 0; i < total_pixels; ++i) {
        float fx = gx[i], fy = gy[i];
        magnitude[i] = sqrtf(fx*fx + fy*fy);
        angle[i] = atan2f(fy, fx);
    }

    // 5. Non-maximum suppression
    stbi_uc *nms = (stbi_uc*)malloc(total_pixels);
    if (!nms) {
        free(magnitude); free(angle);
        free(gx); free(gy);
        free(blurred);
        if (gray_temp) free(gray_temp);
        _STBIE_SET_ERROR(STBIE_ERROR_INVALID_PARAM);
        return 0;
    }
    _stbie_non_max_suppression(magnitude, angle, w, h, nms);

    // 6. Hysteresis thresholding
    _stbie_hysteresis_threshold(nms, w, h, low_thresh, high_thresh, output);

    // Cleanup
    free(nms);
    free(magnitude);
    free(angle);
    free(gx);
    free(gy);
    free(blurred);
    if (gray_temp) free(gray_temp);

    _STBIE_SET_ERROR(STBIE_ERROR_NONE);
    return 1;
}

#endif /* STB_IMAGE_EDIT_IMPLEMENT_ONCE */
#endif /* STB_IMAGE_EDIT_IMPLEMENT */
#endif /* STBIE_EDIT_H_INCLUDED */
