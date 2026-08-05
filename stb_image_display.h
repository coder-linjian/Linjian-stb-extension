/*
 * stb_image_display.h - v1.0.0 - single?header image display library
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
 *   #define STB_IMAGE_DISPLAY_IMPLEMENTATION
 *   #include "stb_image_display.h"
 *
 *   // In exactly ONE C file:
 *   #define STB_IMAGE_DISPLAY_IMPLEMENTATION
 *   #include "stb_image_display.h"
 *
 *   // Example:
 *   if (stbid_Startup() == 0) {
 *       if (stbid_ShowImage(rgb_data, width, height, 3, "Image") != 0) {
 *           printf("Error: %d\n", stbid_GetLastError());
 *       }
 *       stbid_Shutdown();
 *   }
 *
 * COMPILATION:
 *   - Windows (MinGW) : gcc ... -lgdi32 -luser32
 *   - Linux (gcc)     : gcc ... -lX11
 *   - macOS           : NOT SUPPORTED (intentionally)
 *
 * ============================================================================
 *
 * API OVERVIEW:
 *
 *   Core functions:
 *     int  stbid_Startup(void);
 *     int  stbid_ShowImage(const unsigned char* data, int width, int height,
 *                          int channels, const char* title);
 *     void stbid_Shutdown(void);
 *
 *   Non?blocking extensions (Windows fully implemented; Linux stubs):
 *     void* stbid_ShowImageNonBlocking(const unsigned char* data, int width, int height,
 *                                      int channels, const char* title);
 *     int   stbid_UpdateImage(void* handle, const unsigned char* data);
 *     int   stbid_IsWindowOpen(void* handle);
 *     void  stbid_CloseWindow(void* handle);
 *     int   stbid_PollEvents(void);   // returns 1 if quit event, else 0
 *
 *   Windows?only: draw image to any HDC
 *     int stbid_DrawToHDC(HDC hdc, int dstX, int dstY, int dstWidth, int dstHeight,
 *                         const unsigned char* data, int srcWidth, int srcHeight,
 *                         int channels);
 *
 *   Windows?only: capture image from HDC (screen/window/print)
 *     unsigned char* stbid_CopyFromHDC(HDC hdc, int* width, int* height, int* channels);
 *     // Returns pointer to RGB 24-bit data, or NULL on error.
 *     // Caller must free() the returned pointer.
 *
 *   Error handling:
 *     stbid_ErrorCode stbid_GetLastError(void);
 *
 *   Error codes (stbid_ErrorCode):
 *     STBID_SUCCESS                = 0
 *     STBID_ERR_UNKNOWN            = -1
 *     STBID_ERR_PARAM              = -2
 *     STBID_ERR_NOT_INIT           = -3
 *     STBID_ERR_NO_MEM             = -4
 *     STBID_ERR_WINDOW             = -5
 *     STBID_ERR_HDC                = -6
 *     STBID_ERR_PLATFORM_STUB_FUNC = -7
 *
 *   All functions expect 8?bit per channel, interleaved RGB (3 channels)
 *   or RGBA (4 channels) for input. The library handles internal colour conversion.
 *   The blocking ShowImage blocks until the window is closed.
 *   Non?blocking functions return a void* handle to the window.
 *
 * ============================================================================
 */

#ifndef STB_IMAGE_DISPLAY_H_INCLUDED
#define STB_IMAGE_DISPLAY_H_INCLUDED

/* ---- Platform detection (internal) ---- */
#if defined(_WIN32) || defined(_WIN64)
  #define STBID_PLATFORM_WINDOWS
#elif defined(__linux__)
  #define STBID_PLATFORM_LINUX
#elif defined(__APPLE__)
  #error "macOS is not supported by stb_image_display.h"
#else
  #error "Unsupported platform"
#endif

/* ---- Platform-specific includes for declarations ---- */
#ifdef STBID_PLATFORM_WINDOWS
  #include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Error codes ---- */
typedef enum {
    STBID_SUCCESS                = 0,
    STBID_ERR_UNKNOWN            = -1,
    STBID_ERR_PARAM              = -2,
    STBID_ERR_NOT_INIT           = -3,
    STBID_ERR_NO_MEM             = -4,
    STBID_ERR_WINDOW             = -5,
    STBID_ERR_HDC                = -6,
    STBID_ERR_PLATFORM_STUB_FUNC = -7
} stbid_ErrorCode;

/* ---- Core functions ---- */
int  stbid_Startup(void);
int  stbid_ShowImage(const unsigned char* data, int width, int height,
                     int channels, const char* title);
void stbid_Shutdown(void);

/* ---- Extended non?blocking functions ---- */
void* stbid_ShowImageNonBlocking(const unsigned char* data, int width, int height,
                                 int channels, const char* title);
int   stbid_UpdateImage(void* handle, const unsigned char* data);
int   stbid_IsWindowOpen(void* handle);
void  stbid_CloseWindow(void* handle);
int   stbid_PollEvents(void);

/* ---- Windows?only: draw image to any device context ---- */
#ifdef STBID_PLATFORM_WINDOWS
int stbid_DrawToHDC(HDC hdc,
                    int dstX, int dstY, int dstWidth, int dstHeight,
                    const unsigned char* data,
                    int srcWidth, int srcHeight, int channels);
#endif

/* ---- Windows?only: capture image from HDC (returns allocated memory) ---- */
#ifdef STBID_PLATFORM_WINDOWS
unsigned char* stbid_CopyFromHDC(HDC hdc, int* width, int* height, int* channels);
#endif

/* ---- Error query ---- */
stbid_ErrorCode stbid_GetLastError(void);

#ifdef __cplusplus
}
#endif

/* ============================================================================
   Implementation
   ============================================================================ */
#ifdef STB_IMAGE_DISPLAY_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

/* ---- Platform-specific includes for implementation ---- */
#ifdef STBID_PLATFORM_WINDOWS
  #include <windows.h>
#else /* STBID_PLATFORM_LINUX */
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <X11/Xatom.h>
#endif

/* ---- Global error state (single-threaded only) ---- */
static stbid_ErrorCode _g_last_error = STBID_SUCCESS;

#define _STBID_SET_ERROR(code) do { _g_last_error = (code); } while(0)

/* ---- Helper to validate image parameters ---- */
static int _stbid_validate_params(const unsigned char* data, int width, int height, int channels)
{
    if (!data || width <= 0 || height <= 0 || (channels != 3 && channels != 4)) {
        _STBID_SET_ERROR(STBID_ERR_PARAM);
        return 0;
    }
    return 1;
}

/* ----------------------------------------------------------------------------
 * Windows implementation
 * -------------------------------------------------------------------------- */
#ifdef STBID_PLATFORM_WINDOWS

typedef struct {
    int width, height;
    int stride;
    unsigned char* data;   /* BGR 24?bit, top?down, stride aligned to 4 */
    BITMAPINFO* bmi;
} ImageData;

static HINSTANCE _g_hInst = NULL;
static ATOM      _g_atomClass = 0;

static LRESULT CALLBACK _stbid_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            ImageData* p = (ImageData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
            if (p) {
                StretchDIBits(hdc,
                    0, 0, p->width, p->height,
                    0, 0, p->width, p->height,
                    p->data, p->bmi, DIB_RGB_COLORS, SRCCOPY);
            }
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_DESTROY: {
            ImageData* p = (ImageData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
            if (p) {
                if (p->bmi)  free(p->bmi);
                if (p->data) free(p->data);
                free(p);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            }
            PostQuitMessage(0);
            return 0;
        }
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

int stbid_Startup(void)
{
    if (_g_atomClass) {
        _STBID_SET_ERROR(STBID_SUCCESS);
        return 0;
    }
    _g_hInst = GetModuleHandle(NULL);
    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = _stbid_WndProc;
    wc.hInstance     = _g_hInst;
    wc.lpszClassName = "STBID_Window";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    _g_atomClass = RegisterClassEx(&wc);
    if (!_g_atomClass) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }
    _STBID_SET_ERROR(STBID_SUCCESS);
    return 0;
}

void stbid_Shutdown(void)
{
    if (_g_atomClass) {
        UnregisterClass("STBID_Window", _g_hInst);
        _g_atomClass = 0;
    }
    _STBID_SET_ERROR(STBID_SUCCESS);
}

/* ---------- Blocking ShowImage ---------- */
int stbid_ShowImage(const unsigned char* data, int width, int height,
                    int channels, const char* title)
{
    if (!_stbid_validate_params(data, width, height, channels))
        return -1;
    if (!_g_atomClass) {
        _STBID_SET_ERROR(STBID_ERR_NOT_INIT);
        return -1;
    }

    int stride = ((width * 3 + 3) & ~3);
    unsigned char* bgr = (unsigned char*)malloc((size_t)height * stride);
    if (!bgr) {
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return -1;
    }

    for (int y = 0; y < height; ++y) {
        const unsigned char* src = data + (size_t)y * width * channels;
        unsigned char* dst = bgr + (size_t)y * stride;
        for (int x = 0; x < width; ++x) {
            dst[0] = src[2];  /* B */
            dst[1] = src[1];  /* G */
            dst[2] = src[0];  /* R */
            src += channels;
            dst += 3;
        }
    }

    BITMAPINFO* bmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER));
    if (!bmi) {
        free(bgr);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return -1;
    }
    bmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bmi->bmiHeader.biWidth         = width;
    bmi->bmiHeader.biHeight        = -height;
    bmi->bmiHeader.biPlanes        = 1;
    bmi->bmiHeader.biBitCount      = 24;
    bmi->bmiHeader.biCompression   = BI_RGB;
    bmi->bmiHeader.biSizeImage     = 0;
    bmi->bmiHeader.biXPelsPerMeter = 0;
    bmi->bmiHeader.biYPelsPerMeter = 0;
    bmi->bmiHeader.biClrUsed       = 0;
    bmi->bmiHeader.biClrImportant  = 0;

    RECT rect = {0, 0, width, height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    int winW = rect.right - rect.left;
    int winH = rect.bottom - rect.top;

    HWND hWnd = CreateWindowEx(0, "STBID_Window", title ? title : "Image",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               winW, winH,
                               NULL, NULL, _g_hInst, NULL);
    if (!hWnd) {
        free(bmi);
        free(bgr);
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }

    ImageData* pData = (ImageData*)malloc(sizeof(ImageData));
    if (!pData) {
        DestroyWindow(hWnd);
        free(bmi);
        free(bgr);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return -1;
    }
    pData->width  = width;
    pData->height = height;
    pData->stride = stride;
    pData->data   = bgr;
    pData->bmi    = bmi;

    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pData);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    _STBID_SET_ERROR(STBID_SUCCESS);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

/* ---------- Non?blocking extensions ---------- */
void* stbid_ShowImageNonBlocking(const unsigned char* data, int width, int height,
                                 int channels, const char* title)
{
    if (!_stbid_validate_params(data, width, height, channels))
        return NULL;
    if (!_g_atomClass) {
        _STBID_SET_ERROR(STBID_ERR_NOT_INIT);
        return NULL;
    }

    int stride = ((width * 3 + 3) & ~3);
    unsigned char* bgr = (unsigned char*)malloc((size_t)height * stride);
    if (!bgr) {
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return NULL;
    }

    for (int y = 0; y < height; ++y) {
        const unsigned char* src = data + (size_t)y * width * channels;
        unsigned char* dst = bgr + (size_t)y * stride;
        for (int x = 0; x < width; ++x) {
            dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0];
            src += channels; dst += 3;
        }
    }

    BITMAPINFO* bmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER));
    if (!bmi) {
        free(bgr);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return NULL;
    }
    bmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bmi->bmiHeader.biWidth         = width;
    bmi->bmiHeader.biHeight        = -height;
    bmi->bmiHeader.biPlanes        = 1;
    bmi->bmiHeader.biBitCount      = 24;
    bmi->bmiHeader.biCompression   = BI_RGB;
    bmi->bmiHeader.biSizeImage     = 0;
    bmi->bmiHeader.biXPelsPerMeter = 0;
    bmi->bmiHeader.biYPelsPerMeter = 0;
    bmi->bmiHeader.biClrUsed       = 0;
    bmi->bmiHeader.biClrImportant  = 0;

    RECT rect = {0, 0, width, height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    int winW = rect.right - rect.left;
    int winH = rect.bottom - rect.top;

    HWND hWnd = CreateWindowEx(0, "STBID_Window", title ? title : "Image",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               winW, winH,
                               NULL, NULL, _g_hInst, NULL);
    if (!hWnd) {
        free(bmi);
        free(bgr);
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return NULL;
    }

    ImageData* pData = (ImageData*)malloc(sizeof(ImageData));
    if (!pData) {
        DestroyWindow(hWnd);
        free(bmi);
        free(bgr);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return NULL;
    }
    pData->width  = width;
    pData->height = height;
    pData->stride = stride;
    pData->data   = bgr;
    pData->bmi    = bmi;

    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pData);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    _STBID_SET_ERROR(STBID_SUCCESS);
    return (void*)hWnd;
}

int stbid_UpdateImage(void* handle, const unsigned char* data)
{
    HWND hWnd = (HWND)handle;
    if (!IsWindow(hWnd)) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }
    ImageData* pData = (ImageData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (!pData || !data) {
        _STBID_SET_ERROR(STBID_ERR_PARAM);
        return -1;
    }

    int width = pData->width, height = pData->height;
    int stride = pData->stride;
    unsigned char* bgr = pData->data;

    for (int y = 0; y < height; ++y) {
        const unsigned char* src = data + (size_t)y * width * 3;
        unsigned char* dst = bgr + (size_t)y * stride;
        for (int x = 0; x < width; ++x) {
            dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0];
            src += 3; dst += 3;
        }
    }
    InvalidateRect(hWnd, NULL, FALSE);
    _STBID_SET_ERROR(STBID_SUCCESS);
    return 0;
}

int stbid_IsWindowOpen(void* handle)
{
    return IsWindow((HWND)handle) ? 1 : 0;
}

void stbid_CloseWindow(void* handle)
{
    if (IsWindow((HWND)handle))
        PostMessage((HWND)handle, WM_CLOSE, 0, 0);
}

int stbid_PollEvents(void)
{
    MSG msg;
    int quit = 0;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            quit = 1;
            PostQuitMessage(0);
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return quit;
}

/* ---------- DrawToHDC ---------- */
int stbid_DrawToHDC(HDC hdc,
                    int dstX, int dstY, int dstWidth, int dstHeight,
                    const unsigned char* data,
                    int srcWidth, int srcHeight, int channels)
{
    if (!_stbid_validate_params(data, srcWidth, srcHeight, channels))
        return -1;
    if (!hdc) {
        _STBID_SET_ERROR(STBID_ERR_HDC);
        return -1;
    }

    int stride = ((srcWidth * 3 + 3) & ~3);
    unsigned char* bgr = (unsigned char*)malloc((size_t)srcHeight * stride);
    if (!bgr) {
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return -1;
    }

    for (int y = 0; y < srcHeight; ++y) {
        const unsigned char* src = data + (size_t)y * srcWidth * channels;
        unsigned char* dst = bgr + (size_t)y * stride;
        for (int x = 0; x < srcWidth; ++x) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            src += channels;
            dst += 3;
        }
    }

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth         = srcWidth;
    bmi.bmiHeader.biHeight        = -srcHeight;
    bmi.bmiHeader.biPlanes        = 1;
    bmi.bmiHeader.biBitCount      = 24;
    bmi.bmiHeader.biCompression   = BI_RGB;
    bmi.bmiHeader.biSizeImage     = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed       = 0;
    bmi.bmiHeader.biClrImportant  = 0;

    int result = StretchDIBits(hdc,
                               dstX, dstY, dstWidth, dstHeight,
                               0, 0, srcWidth, srcHeight,
                               bgr, &bmi, DIB_RGB_COLORS, SRCCOPY);
    free(bgr);

    if (result == GDI_ERROR) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }
    _STBID_SET_ERROR(STBID_SUCCESS);
    return 0;
}

/* ---------- CopyFromHDC (returns allocated memory) ---------- */
unsigned char* stbid_CopyFromHDC(HDC hdc, int* width, int* height, int* channels)
{
    if (!hdc) {
        _STBID_SET_ERROR(STBID_ERR_HDC);
        return NULL;
    }
    if (!width || !height || !channels) {
        _STBID_SET_ERROR(STBID_ERR_PARAM);
        return NULL;
    }

    int w = GetDeviceCaps(hdc, HORZRES);
    int h = GetDeviceCaps(hdc, VERTRES);
    if (w <= 0 || h <= 0) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return NULL;
    }

    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return NULL;
    }

    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, w, h);
    if (!hBitmap) {
        DeleteDC(memDC);
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return NULL;
    }

    HGDIOBJ oldBmp = SelectObject(memDC, hBitmap);
    if (!BitBlt(memDC, 0, 0, w, h, hdc, 0, 0, SRCCOPY)) {
        SelectObject(memDC, oldBmp);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return NULL;
    }

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth         = w;
    bmi.bmiHeader.biHeight        = -h;
    bmi.bmiHeader.biPlanes        = 1;
    bmi.bmiHeader.biBitCount      = 24;
    bmi.bmiHeader.biCompression   = BI_RGB;
    bmi.bmiHeader.biSizeImage     = 0;

    int stride = ((w * 3 + 3) & ~3);
    unsigned char* dib_data = (unsigned char*)malloc((size_t)h * stride);
    if (!dib_data) {
        SelectObject(memDC, oldBmp);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return NULL;
    }

    int result = GetDIBits(memDC, hBitmap, 0, h, dib_data, &bmi, DIB_RGB_COLORS);
    if (result == 0 || result == ERROR_INVALID_PARAMETER) {
        free(dib_data);
        SelectObject(memDC, oldBmp);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return NULL;
    }

    unsigned char* rgb_data = (unsigned char*)malloc((size_t)h * w * 3);
    if (!rgb_data) {
        free(dib_data);
        SelectObject(memDC, oldBmp);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return NULL;
    }

    for (int y = 0; y < h; ++y) {
        unsigned char* src = dib_data + (size_t)y * stride;
        unsigned char* dst = rgb_data + (size_t)y * w * 3;
        for (int x = 0; x < w; ++x) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            src += 3;
            dst += 3;
        }
    }

    free(dib_data);
    SelectObject(memDC, oldBmp);
    DeleteObject(hBitmap);
    DeleteDC(memDC);

    *width = w;
    *height = h;
    *channels = 3;
    _STBID_SET_ERROR(STBID_SUCCESS);
    return rgb_data;
}

#endif /* STBID_PLATFORM_WINDOWS */

/* ----------------------------------------------------------------------------
 * Linux (X11) implementation
 * -------------------------------------------------------------------------- */
#ifdef STBID_PLATFORM_LINUX

static Display* _g_disp = NULL;
static int      _g_screen = 0;
static Atom     _g_wmDelete = None;

int stbid_Startup(void)
{
    if (_g_disp) {
        _STBID_SET_ERROR(STBID_SUCCESS);
        return 0;
    }
    _g_disp = XOpenDisplay(NULL);
    if (!_g_disp) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }
    _g_screen = DefaultScreen(_g_disp);
    _g_wmDelete = XInternAtom(_g_disp, "WM_DELETE_WINDOW", False);
    _STBID_SET_ERROR(STBID_SUCCESS);
    return 0;
}

void stbid_Shutdown(void)
{
    if (_g_disp) {
        XCloseDisplay(_g_disp);
        _g_disp = NULL;
    }
    _STBID_SET_ERROR(STBID_SUCCESS);
}

/* helper to find TrueColor visual */
static Visual* _stbid_find_truecolor_visual(int depth)
{
    XVisualInfo templ;
    templ.screen = _g_screen;
    templ.depth  = depth;
    templ.class  = TrueColor;
    int count;
    XVisualInfo* vi = XGetVisualInfo(_g_disp,
                        VisualScreenMask | VisualDepthMask | VisualClassMask,
                        &templ, &count);
    Visual* vis = NULL;
    if (vi && count > 0) {
        vis = vi->visual;
        XFree(vi);
    }
    return vis;
}

/* ---------- Blocking ShowImage ---------- */
int stbid_ShowImage(const unsigned char* data, int width, int height,
                    int channels, const char* title)
{
    if (!_stbid_validate_params(data, width, height, channels))
        return -1;
    if (!_g_disp) {
        _STBID_SET_ERROR(STBID_ERR_NOT_INIT);
        return -1;
    }

    Visual* visual = _stbid_find_truecolor_visual(24);
    int depth = 24;
    if (!visual) {
        visual = _stbid_find_truecolor_visual(32);
        depth = 32;
    }
    if (!visual) {
        visual = DefaultVisual(_g_disp, _g_screen);
        depth  = DefaultDepth(_g_disp, _g_screen);
    }

    Window win = XCreateSimpleWindow(_g_disp, RootWindow(_g_disp, _g_screen),
                                     0, 0, width, height, 1,
                                     BlackPixel(_g_disp, _g_screen),
                                     WhitePixel(_g_disp, _g_screen));
    if (!win) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }

    if (title) XStoreName(_g_disp, win, title);
    XSetWMProtocols(_g_disp, win, &_g_wmDelete, 1);
    XSelectInput(_g_disp, win, ExposureMask | KeyPressMask |
                              ButtonPressMask | StructureNotifyMask);

    XImage* image = XCreateImage(_g_disp, visual, depth, ZPixmap, 0, NULL,
                                 width, height, 32, 0);
    if (!image) {
        XDestroyWindow(_g_disp, win);
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }

    size_t bytes_per_line = image->bytes_per_line;
    unsigned char* pixbuf = (unsigned char*)malloc((size_t)height * bytes_per_line);
    if (!pixbuf) {
        XDestroyImage(image);
        XDestroyWindow(_g_disp, win);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return -1;
    }
    image->data = (char*)pixbuf;

    unsigned long rmask = image->red_mask;
    unsigned long gmask = image->green_mask;
    unsigned long bmask = image->blue_mask;
    int rshift = 0, gshift = 0, bshift = 0;
    unsigned long tmp;
    tmp = rmask; while ((tmp & 1) == 0) { rshift++; tmp >>= 1; }
    tmp = gmask; while ((tmp & 1) == 0) { gshift++; tmp >>= 1; }
    tmp = bmask; while ((tmp & 1) == 0) { bshift++; tmp >>= 1; }

    for (int y = 0; y < height; ++y) {
        const unsigned char* src = data + (size_t)y * width * channels;
        for (int x = 0; x < width; ++x) {
            unsigned long pixel;
            if (channels == 3) {
                pixel = ((src[0] << rshift) & rmask) |
                        ((src[1] << gshift) & gmask) |
                        ((src[2] << bshift) & bmask);
            } else {
                pixel = ((src[0] << rshift) & rmask) |
                        ((src[1] << gshift) & gmask) |
                        ((src[2] << bshift) & bmask);
            }
            XPutPixel(image, x, y, pixel);
            src += channels;
        }
    }

    GC gc = XCreateGC(_g_disp, win, 0, NULL);
    XMapWindow(_g_disp, win);
    XFlush(_g_disp);

    _STBID_SET_ERROR(STBID_SUCCESS);

    XEvent ev;
    int done = 0;
    while (!done) {
        XNextEvent(_g_disp, &ev);
        switch (ev.type) {
            case Expose:
                if (ev.xexpose.count == 0) {
                    XPutImage(_g_disp, win, gc, image, 0, 0, 0, 0, width, height);
                }
                break;
            case KeyPress:
            case ButtonPress:
                done = 1;
                break;
            case ClientMessage:
                if ((Atom)ev.xclient.data.l[0] == _g_wmDelete)
                    done = 1;
                break;
            case DestroyNotify:
                done = 1;
                break;
        }
    }

    XDestroyWindow(_g_disp, win);
    XFreeGC(_g_disp, gc);
    XDestroyImage(image);
    return 0;
}

/* ---------- Extended functions (stubs for Linux) ---------- */
void* stbid_ShowImageNonBlocking(const unsigned char* data, int width, int height,
                                 int channels, const char* title)
{
    (void)data; (void)width; (void)height; (void)channels; (void)title;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return NULL;
}

int stbid_UpdateImage(void* handle, const unsigned char* data)
{
    (void)handle; (void)data;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return -1;
}

int stbid_IsWindowOpen(void* handle)
{
    (void)handle;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return 0;
}

void stbid_CloseWindow(void* handle)
{
    (void)handle;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
}

int stbid_PollEvents(void)
{
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return 0;
}

#endif /* STBID_PLATFORM_LINUX */

/* ---- Error query ---- */
stbid_ErrorCode stbid_GetLastError(void)
{
    return _g_last_error;
}

#endif /* STB_IMAGE_DISPLAY_IMPLEMENTATION */

#endif /* STB_IMAGE_DISPLAY_H_INCLUDED */
