/*
 * stb_image_display.h - v1.1.0 - single header image display library
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
 * API OVERVIEW (v1.1.0):
 *
 *   Core functions:
 *     int  stbid_Startup(void);
 *     int  stbid_ShowImage(const unsigned char* data, int width, int height,
 *                          int channels, const char* title);
 *     void stbid_Shutdown(void);
 *
 *   Non?blocking extensions (Windows fully implemented; Linux stubs):
 *     void* stbid_ShowImageNonBlocking(...);
 *     int   stbid_UpdateImage(void* handle, const unsigned char* data);
 *     int   stbid_IsWindowOpen(void* handle);
 *     void  stbid_CloseWindow(void* handle);
 *     int   stbid_PollEvents(void);
 *
 *   New in v1.1.0:
 *     // Window scaling modes
 *     typedef enum { STBID_SCALE_STRETCH, STBID_SCALE_KEEP_ASPECT, STBID_SCALE_CENTER } stbid_ScaleMode;
 *     int stbid_SetScaleMode(void* handle, stbid_ScaleMode mode);
 *
 *     // Keyboard input (blocking, similar to cv::waitKey)
 *     int stbid_WaitKey(int timeout_ms);   // 0 = infinite, negative = immediate
 *
 *     // Multi?window management
 *     int   stbid_GetWindowCount(void);
 *     void* stbid_GetWindowHandle(int index);
 *     void  stbid_BringToFront(void* handle);
 *     void  stbid_SetWindowTitle(void* handle, const char* title);
 *
 *   Windows?only draw/capture:
 *     int stbid_DrawToHDC(HDC hdc, int dstX, int dstY, int dstWidth, int dstHeight,
 *                         const unsigned char* data, int srcWidth, int srcHeight, int channels);
 *     int stbid_CopyFromHDC(HDC hdc, unsigned char** out_data,
 *                           int* width, int* height, int* channels);
 *     // Returns 0 on success, -1 on error; out_data is malloc'd, caller must free().
 *
 *   Error handling:
 *     stbid_ErrorCode stbid_GetLastError(void);
 *
 * ============================================================================
 */

#ifndef STB_IMAGE_DISPLAY_H_INCLUDED
#define STB_IMAGE_DISPLAY_H_INCLUDED

/* ---- Platform detection ---- */
#if defined(_WIN32) || defined(_WIN64)
  #define STBID_PLATFORM_WINDOWS
#elif defined(__linux__)
  #define STBID_PLATFORM_LINUX
#elif defined(__APPLE__)
  #error "macOS is not supported by stb_image_display.h"
#else
  #error "Unsupported platform"
#endif

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

/* ---- Scale mode ---- */
typedef enum {
    STBID_SCALE_STRETCH,      /* stretch to fill window */
    STBID_SCALE_KEEP_ASPECT,  /* keep aspect ratio, centered */
    STBID_SCALE_CENTER        /* original size, centered */
} stbid_ScaleMode;

/* ---- Core ---- */
int  stbid_Startup(void);
int  stbid_ShowImage(const unsigned char* data, int width, int height,
                     int channels, const char* title);
void stbid_Shutdown(void);

/* ---- Non?blocking ---- */
void* stbid_ShowImageNonBlocking(const unsigned char* data, int width, int height,
                                 int channels, const char* title);
int   stbid_UpdateImage(void* handle, const unsigned char* data);
int   stbid_IsWindowOpen(void* handle);
void  stbid_CloseWindow(void* handle);
int   stbid_PollEvents(void);

/* ---- New in v1.1.0: Scaling ---- */
int stbid_SetScaleMode(void* handle, stbid_ScaleMode mode);

/* ---- New in v1.1.0: Keyboard input (blocking) ---- */
int stbid_WaitKey(int timeout_ms);   /* 0 = infinite, negative = immediate */

/* ---- New in v1.1.0: Multi?window management ---- */
int   stbid_GetWindowCount(void);
void* stbid_GetWindowHandle(int index);
void  stbid_BringToFront(void* handle);
void  stbid_SetWindowTitle(void* handle, const char* title);

/* ---- Windows?only ---- */
#ifdef STBID_PLATFORM_WINDOWS
int stbid_DrawToHDC(HDC hdc, int dstX, int dstY, int dstWidth, int dstHeight,
                    const unsigned char* data, int srcWidth, int srcHeight, int channels);
int stbid_CopyFromHDC(HDC hdc, unsigned char** out_data,
                      int* width, int* height, int* channels);
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

#ifdef STBID_PLATFORM_WINDOWS
  #include <windows.h>
#else
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <X11/Xatom.h>
#endif

static stbid_ErrorCode _g_last_error = STBID_SUCCESS;
#define _STBID_SET_ERROR(code) do { _g_last_error = (code); } while(0)

static int _stbid_validate_params(const unsigned char* data, int width, int height, int channels) {
    if (!data || width <= 0 || height <= 0 || (channels != 3 && channels != 4)) {
        _STBID_SET_ERROR(STBID_ERR_PARAM);
        return 0;
    }
    return 1;
}

/* --------------------------------------------------------------------------
   Windows Implementation
   -------------------------------------------------------------------------- */
#ifdef STBID_PLATFORM_WINDOWS

/* ----- Window context ----- */
typedef struct {
    int width, height;
    int stride;
    unsigned char* data;        /* BGR 24?bit, top?down */
    BITMAPINFO* bmi;
    stbid_ScaleMode scale_mode;
    RECT client_rect;
    int win_width, win_height;
} ImageData;

/* ----- Global state for multi?window and keyboard ----- */
static HINSTANCE _g_hInst = NULL;
static ATOM      _g_atomClass = 0;
static void**    _g_windows = NULL;      /* array of HWNDs */
static int       _g_window_count = 0;
static int       _g_window_capacity = 0;

/* Key queue (for WaitKey) */
#define MAX_KEY_QUEUE 256
static int _g_key_queue[MAX_KEY_QUEUE];
static int _g_key_queue_head = 0;
static int _g_key_queue_tail = 0;
static int _g_key_queue_count = 0;
static HANDLE _g_key_event = NULL;   /* event for signaling WaitKey */

static void _stbid_add_window(HWND hwnd) {
    if (_g_window_count >= _g_window_capacity) {
        int new_cap = _g_window_capacity ? _g_window_capacity * 2 : 8;
        void** new_arr = (void**)realloc(_g_windows, new_cap * sizeof(void*));
        if (!new_arr) return;
        _g_windows = new_arr;
        _g_window_capacity = new_cap;
    }
    _g_windows[_g_window_count++] = (void*)hwnd;
}

static void _stbid_remove_window(HWND hwnd) {
    for (int i = 0; i < _g_window_count; ++i) {
        if (_g_windows[i] == (void*)hwnd) {
            for (int j = i; j < _g_window_count - 1; ++j)
                _g_windows[j] = _g_windows[j+1];
            _g_window_count--;
            return;
        }
    }
}

static void _stbid_push_key(int key) {
    if (_g_key_queue_count < MAX_KEY_QUEUE) {
        _g_key_queue[_g_key_queue_tail] = key;
        _g_key_queue_tail = (_g_key_queue_tail + 1) % MAX_KEY_QUEUE;
        _g_key_queue_count++;
        if (_g_key_event) SetEvent(_g_key_event);
    }
}

static int _stbid_pop_key(void) {
    if (_g_key_queue_count == 0) return -1;
    int key = _g_key_queue[_g_key_queue_head];
    _g_key_queue_head = (_g_key_queue_head + 1) % MAX_KEY_QUEUE;
    _g_key_queue_count--;
    return key;
}

/* ----- Window procedure ----- */
static LRESULT CALLBACK _stbid_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    ImageData* p = (ImageData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    switch (msg) {
        case WM_CREATE: {
            if (!_g_key_event) _g_key_event = CreateEvent(NULL, FALSE, FALSE, NULL);
            break;
        }
        case WM_SIZE: {
            if (p) {
                GetClientRect(hWnd, &p->client_rect);
                p->win_width = p->client_rect.right - p->client_rect.left;
                p->win_height = p->client_rect.bottom - p->client_rect.top;
            }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (p && p->data) {
            	SetStretchBltMode(hdc, HALFTONE);
        		SetBrushOrgEx(hdc, 0, 0, NULL);
                int dstW = p->win_width, dstH = p->win_height;
                int x = 0, y = 0;
                int srcW = p->width, srcH = p->height;
                switch (p->scale_mode) {
                    case STBID_SCALE_STRETCH:
                        break;
                    case STBID_SCALE_KEEP_ASPECT: {
                        float ar_src = (float)srcW / srcH;
                        float ar_dst = (float)dstW / dstH;
                        if (ar_src > ar_dst) {
                            dstH = (int)(dstW / ar_src);
                            y = (p->win_height - dstH) / 2;
                        } else {
                            dstW = (int)(dstH * ar_src);
                            x = (p->win_width - dstW) / 2;
                        }
                        break;
                    }
                    case STBID_SCALE_CENTER:
                        dstW = srcW;
                        dstH = srcH;
                        x = (p->win_width - dstW) / 2;
                        y = (p->win_height - dstH) / 2;
                        break;
                }
                StretchDIBits(hdc, x, y, dstW, dstH,
                              0, 0, srcW, srcH,
                              p->data, p->bmi, DIB_RGB_COLORS, SRCCOPY);
            }
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_KEYDOWN: {
            int vk = (int)wParam;
            int key = -1;
            if (vk >= 'A' && vk <= 'Z') key = vk;
            else if (vk >= '0' && vk <= '9') key = vk;
            else if (vk == VK_ESCAPE) key = 27;
            else if (vk == VK_RETURN) key = 13;
            else if (vk == VK_SPACE) key = 32;
            else if (vk == VK_LEFT) key = 0x2500;
            else if (vk == VK_UP) key = 0x2501;
            else if (vk == VK_RIGHT) key = 0x2502;
            else if (vk == VK_DOWN) key = 0x2503;
            if (key != -1) _stbid_push_key(key);
            break;
        }
        case WM_DESTROY: {
            if (p) {
                if (p->bmi) free(p->bmi);
                if (p->data) free(p->data);
                free(p);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            }
            _stbid_remove_window(hWnd);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

/* ----- Startup / Shutdown ----- */
int stbid_Startup(void) {
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
    if (!_g_key_event) _g_key_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    _STBID_SET_ERROR(STBID_SUCCESS);
    return 0;
}

void stbid_Shutdown(void) {
    // Close all windows
    for (int i = _g_window_count - 1; i >= 0; --i) {
        HWND hwnd = (HWND)_g_windows[i];
        if (IsWindow(hwnd)) DestroyWindow(hwnd);
    }
    free(_g_windows);
    _g_windows = NULL;
    _g_window_count = 0;
    _g_window_capacity = 0;
    if (_g_key_event) {
        CloseHandle(_g_key_event);
        _g_key_event = NULL;
    }
    if (_g_atomClass) {
        UnregisterClass("STBID_Window", _g_hInst);
        _g_atomClass = 0;
    }
    _STBID_SET_ERROR(STBID_SUCCESS);
}

/* ----- Helper: create window (used by both blocking and non-blocking) ----- */
static HWND _stbid_create_window(const unsigned char* data, int w, int h, int channels,
                                 const char* title, int* out_stride, BITMAPINFO** out_bmi,
                                 unsigned char** out_bgr) {
    int stride = ((w * 3 + 3) & ~3);
    unsigned char* bgr = (unsigned char*)malloc((size_t)h * stride);
    if (!bgr) {
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return NULL;
    }
    // Convert RGB->BGR
    for (int y = 0; y < h; ++y) {
        const unsigned char* src = data + (size_t)y * w * channels;
        unsigned char* dst = bgr + (size_t)y * stride;
        for (int x = 0; x < w; ++x) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            src += channels;
            dst += 3;
        }
    }

    BITMAPINFO* bmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER));
    if (!bmi) {
        free(bgr);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return NULL;
    }
    bmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bmi->bmiHeader.biWidth         = w;
    bmi->bmiHeader.biHeight        = -h;
    bmi->bmiHeader.biPlanes        = 1;
    bmi->bmiHeader.biBitCount      = 24;
    bmi->bmiHeader.biCompression   = BI_RGB;
    bmi->bmiHeader.biSizeImage     = 0;
    bmi->bmiHeader.biXPelsPerMeter = 0;
    bmi->bmiHeader.biYPelsPerMeter = 0;
    bmi->bmiHeader.biClrUsed       = 0;
    bmi->bmiHeader.biClrImportant  = 0;

    RECT rect = {0, 0, w, h};
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

    ImageData* p = (ImageData*)malloc(sizeof(ImageData));
    if (!p) {
        DestroyWindow(hWnd);
        free(bmi);
        free(bgr);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return NULL;
    }
    p->width  = w;
    p->height = h;
    p->stride = stride;
    p->data   = bgr;
    p->bmi    = bmi;
    p->scale_mode = STBID_SCALE_STRETCH;
    p->win_width  = w;
    p->win_height = h;
    GetClientRect(hWnd, &p->client_rect);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)p);
    _stbid_add_window(hWnd);

    if (out_stride) *out_stride = stride;
    if (out_bmi) *out_bmi = bmi;
    if (out_bgr) *out_bgr = bgr;
    return hWnd;
}

/* ----- Blocking ShowImage ----- */
int stbid_ShowImage(const unsigned char* data, int width, int height,
                    int channels, const char* title) {
    if (!_stbid_validate_params(data, width, height, channels))
        return -1;
    if (!_g_atomClass) {
        _STBID_SET_ERROR(STBID_ERR_NOT_INIT);
        return -1;
    }
    HWND hWnd = _stbid_create_window(data, width, height, channels, title, NULL, NULL, NULL);
    if (!hWnd) return -1;
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

/* ----- Non?blocking ----- */
void* stbid_ShowImageNonBlocking(const unsigned char* data, int width, int height,
                                 int channels, const char* title) {
    if (!_stbid_validate_params(data, width, height, channels))
        return NULL;
    if (!_g_atomClass) {
        _STBID_SET_ERROR(STBID_ERR_NOT_INIT);
        return NULL;
    }
    HWND hWnd = _stbid_create_window(data, width, height, channels, title, NULL, NULL, NULL);
    if (!hWnd) return NULL;
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    _STBID_SET_ERROR(STBID_SUCCESS);
    return (void*)hWnd;
}

int stbid_UpdateImage(void* handle, const unsigned char* data) {
    HWND hWnd = (HWND)handle;
    if (!IsWindow(hWnd)) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }
    ImageData* p = (ImageData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (!p || !data) {
        _STBID_SET_ERROR(STBID_ERR_PARAM);
        return -1;
    }
    int w = p->width, h = p->height, stride = p->stride;
    unsigned char* bgr = p->data;
    for (int y = 0; y < h; ++y) {
        const unsigned char* src = data + (size_t)y * w * 3;
        unsigned char* dst = bgr + (size_t)y * stride;
        for (int x = 0; x < w; ++x) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            src += 3;
            dst += 3;
        }
    }
    InvalidateRect(hWnd, NULL, FALSE);
    _STBID_SET_ERROR(STBID_SUCCESS);
    return 0;
}

int stbid_IsWindowOpen(void* handle) {
    return IsWindow((HWND)handle) ? 1 : 0;
}

void stbid_CloseWindow(void* handle) {
    HWND hWnd = (HWND)handle;
    if (IsWindow(hWnd)) PostMessage(hWnd, WM_CLOSE, 0, 0);
}

int stbid_PollEvents(void) {
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

/* ----- New v1.1.0 functions ----- */
int stbid_SetScaleMode(void* handle, stbid_ScaleMode mode) {
    HWND hWnd = (HWND)handle;
    if (!IsWindow(hWnd)) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }
    ImageData* p = (ImageData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (!p) {
        _STBID_SET_ERROR(STBID_ERR_PARAM);
        return -1;
    }
    p->scale_mode = mode;
    InvalidateRect(hWnd, NULL, FALSE);
    _STBID_SET_ERROR(STBID_SUCCESS);
    return 0;
}

int stbid_WaitKey(int timeout_ms) {
    if (!_g_key_event) {
        _STBID_SET_ERROR(STBID_ERR_NOT_INIT);
        return -1;
    }
    // First check queue
    int key = _stbid_pop_key();
    if (key != -1) return key;
    if (timeout_ms < 0) return -1;   // immediate return

    DWORD start = GetTickCount();
    while (1) {
        // Process messages to keep window responsive
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        key = _stbid_pop_key();
        if (key != -1) return key;
        if (timeout_ms != 0) {
            DWORD elapsed = GetTickCount() - start;
            if (elapsed >= (DWORD)timeout_ms) break;
        }
        Sleep(10);
    }
    return -1;
}

int stbid_GetWindowCount(void) {
    return _g_window_count;
}

void* stbid_GetWindowHandle(int index) {
    if (index < 0 || index >= _g_window_count) {
        _STBID_SET_ERROR(STBID_ERR_PARAM);
        return NULL;
    }
    return _g_windows[index];
}

void stbid_BringToFront(void* handle) {
    HWND hWnd = (HWND)handle;
    if (IsWindow(hWnd)) {
        SetForegroundWindow(hWnd);
        SetFocus(hWnd);
    } else {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
    }
}

void stbid_SetWindowTitle(void* handle, const char* title) {
    HWND hWnd = (HWND)handle;
    if (IsWindow(hWnd)) {
        SetWindowText(hWnd, title ? title : "");
    } else {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
    }
}

/* ----- Windows?only: DrawToHDC ----- */
int stbid_DrawToHDC(HDC hdc,
                    int dstX, int dstY, int dstWidth, int dstHeight,
                    const unsigned char* data,
                    int srcWidth, int srcHeight, int channels) {
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

/* ----- Windows?only: CopyFromHDC (returns int, out_data via parameter) ----- */
int stbid_CopyFromHDC(HDC hdc, unsigned char** out_data,
                      int* width, int* height, int* channels) {
    if (!hdc) {
        _STBID_SET_ERROR(STBID_ERR_HDC);
        return -1;
    }
    if (!out_data || !width || !height || !channels) {
        _STBID_SET_ERROR(STBID_ERR_PARAM);
        return -1;
    }

    int w = GetDeviceCaps(hdc, HORZRES);
    int h = GetDeviceCaps(hdc, VERTRES);
    if (w <= 0 || h <= 0) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }

    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC) {
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }

    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, w, h);
    if (!hBitmap) {
        DeleteDC(memDC);
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }

    HGDIOBJ oldBmp = SelectObject(memDC, hBitmap);
    if (!BitBlt(memDC, 0, 0, w, h, hdc, 0, 0, SRCCOPY)) {
        SelectObject(memDC, oldBmp);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
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
        return -1;
    }

    int result = GetDIBits(memDC, hBitmap, 0, h, dib_data, &bmi, DIB_RGB_COLORS);
    if (result == 0 || result == ERROR_INVALID_PARAMETER) {
        free(dib_data);
        SelectObject(memDC, oldBmp);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        _STBID_SET_ERROR(STBID_ERR_WINDOW);
        return -1;
    }

    unsigned char* rgb_data = (unsigned char*)malloc((size_t)h * w * 3);
    if (!rgb_data) {
        free(dib_data);
        SelectObject(memDC, oldBmp);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        _STBID_SET_ERROR(STBID_ERR_NO_MEM);
        return -1;
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

    *out_data = rgb_data;
    *width  = w;
    *height = h;
    *channels = 3;
    _STBID_SET_ERROR(STBID_SUCCESS);
    return 0;
}

#endif /* STBID_PLATFORM_WINDOWS */

/* --------------------------------------------------------------------------
   Linux (X11) Implementation (only blocking ShowImage is fully implemented)
   -------------------------------------------------------------------------- */
#ifdef STBID_PLATFORM_LINUX

static Display* _g_disp = NULL;
static int      _g_screen = 0;
static Atom     _g_wmDelete = None;

int stbid_Startup(void) {
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

void stbid_Shutdown(void) {
    if (_g_disp) {
        XCloseDisplay(_g_disp);
        _g_disp = NULL;
    }
    _STBID_SET_ERROR(STBID_SUCCESS);
}

static Visual* _stbid_find_truecolor_visual(int depth) {
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

/* Blocking ShowImage (full implementation) */
int stbid_ShowImage(const unsigned char* data, int width, int height,
                    int channels, const char* title) {
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

/* ---- Stubs for non?blocking and new features ---- */
void* stbid_ShowImageNonBlocking(const unsigned char* data, int width, int height,
                                 int channels, const char* title) {
    (void)data; (void)width; (void)height; (void)channels; (void)title;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return NULL;
}

int stbid_UpdateImage(void* handle, const unsigned char* data) {
    (void)handle; (void)data;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return -1;
}

int stbid_IsWindowOpen(void* handle) {
    (void)handle;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return 0;
}

void stbid_CloseWindow(void* handle) {
    (void)handle;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
}

int stbid_PollEvents(void) {
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return 0;
}

int stbid_SetScaleMode(void* handle, stbid_ScaleMode mode) {
    (void)handle; (void)mode;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return -1;
}

int stbid_WaitKey(int timeout_ms) {
    (void)timeout_ms;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return -1;
}

int stbid_GetWindowCount(void) {
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return 0;
}

void* stbid_GetWindowHandle(int index) {
    (void)index;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
    return NULL;
}

void stbid_BringToFront(void* handle) {
    (void)handle;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
}

void stbid_SetWindowTitle(void* handle, const char* title) {
    (void)handle; (void)title;
    _STBID_SET_ERROR(STBID_ERR_PLATFORM_STUB_FUNC);
}

#endif /* STBID_PLATFORM_LINUX */

/* ---- Error query ---- */
stbid_ErrorCode stbid_GetLastError(void) {
    return _g_last_error;
}

#endif /* STB_IMAGE_DISPLAY_IMPLEMENTATION */

#endif /* STB_IMAGE_DISPLAY_H_INCLUDED */
