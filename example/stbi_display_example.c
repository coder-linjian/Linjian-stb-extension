/*
 * stbi_display_example.c - C language example for stb_image_display.h v1.1.0
 *
 * Demonstrates:
 *   - Blocking and non?blocking window display
 *   - Scaling modes (stretch, keep aspect, center)
 *   - Keyboard input (WaitKey)
 *   - Multi?window management (count, bring to front, set title)
 *   - Windows?only screen capture (CopyFromHDC)
 *
 * Compile (Windows MinGW):
 *   gcc stbi_display_example.c -o example.exe -lgdi32 -luser32
 *
 * Compile (Linux):
 *   gcc stbi_display_example.c -o example -lX11
 *
 * Dependencies:
 *   - stb_image.h (https://github.com/nothings/stb)
 *   - stb_image_display.h (this library)
 *
 * Place input.jpg in the same directory (or provide full path).
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_DISPLAY_IMPLEMENTATION
#include "stb_image_display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x) * 1000)
#endif

/* ---- Helper: generate a colourful gradient (fallback) ---- */
static unsigned char* make_gradient(int w, int h, int channels) {
    unsigned char* data = (unsigned char*)malloc((size_t)w * h * channels);
    if (!data) return NULL;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned char* p = data + (size_t)(y * w + x) * channels;
            p[0] = (unsigned char)(x * 255 / w);
            p[1] = (unsigned char)(y * 255 / h);
            p[2] = (unsigned char)((x + y) * 255 / (w + h));
            if (channels == 4) p[3] = 255;
        }
    }
    return data;
}

/* ---- Load image from file or generate fallback ---- */
static unsigned char* load_image(const char* filename, int* w, int* h, int* channels) {
    unsigned char* img = stbi_load(filename, w, h, channels, 3); // force RGB
    if (img) {
        printf("Loaded '%s': %dx%d, 3 channels\n", filename, *w, *h);
        return img;
    }
    fprintf(stderr, "Failed to load '%s', using synthetic gradient.\n", filename);
    *w = 400; *h = 300; *channels = 3;
    return make_gradient(*w, *h, *channels);
}

/* ---- Example 1: Basic blocking display ---- */
static void example_basic(const unsigned char* img, int w, int h, int channels) {
    printf("\n--- Basic blocking display ---\n");
    stbid_ShowImage(img, w, h, channels, "Blocking: input.jpg (close to continue)");
}

/* ---- Example 2: Non?blocking with scaling and WaitKey ---- */
static void example_scaling_and_key(const unsigned char* img, int w, int h, int channels) {
    printf("\n--- Non?blocking with scaling & WaitKey ---\n");
    unsigned char* copy = (unsigned char*)malloc((size_t)w * h * 3);
    if (!copy) return;
    memcpy(copy, img, (size_t)w * h * 3);

    void* hwnd = stbid_ShowImageNonBlocking(copy, w, h, 3, "Scaling Demo");
    if (!hwnd) {
        fprintf(stderr, "Create window error: %d\n", stbid_GetLastError());
        free(copy);
        return;
    }
    stbid_SetScaleMode(hwnd, STBID_SCALE_KEEP_ASPECT);

    printf("Press '1' STRETCH, '2' KEEP_ASPECT, '3' CENTER, ESC to exit.\n");
    int running = 1;
    while (running && stbid_IsWindowOpen(hwnd)) {
        if (stbid_PollEvents()) break;
        int key = stbid_WaitKey(50);
        if (key != -1) {
            switch (key) {
                case '1': stbid_SetScaleMode(hwnd, STBID_SCALE_STRETCH);
                          printf("Mode: STRETCH\n"); break;
                case '2': stbid_SetScaleMode(hwnd, STBID_SCALE_KEEP_ASPECT);
                          printf("Mode: KEEP_ASPECT\n"); break;
                case '3': stbid_SetScaleMode(hwnd, STBID_SCALE_CENTER);
                          printf("Mode: CENTER\n"); break;
                case 27:  running = 0; break;
                default:  printf("Key pressed: %d\n", key);
            }
        }
        Sleep(10);
    }
    stbid_CloseWindow(hwnd);
    free(copy);
}

/* ---- Example 3: Multi?window management ---- */
static void example_multi_window(const unsigned char* img, int w, int h, int channels) {
    printf("\n--- Multi?window management ---\n");
    unsigned char* copy1 = (unsigned char*)malloc((size_t)w * h * 3);
    unsigned char* copy2 = (unsigned char*)malloc((size_t)w * h * 3);
    if (!copy1 || !copy2) {
        free(copy1);
        free(copy2);
        return;
    }
    memcpy(copy1, img, (size_t)w * h * 3);
    memcpy(copy2, img, (size_t)w * h * 3);

    void* win1 = stbid_ShowImageNonBlocking(copy1, w, h, 3, "Window 1");
    void* win2 = stbid_ShowImageNonBlocking(copy2, w, h, 3, "Window 2");
    if (!win1 || !win2) {
        fprintf(stderr, "Failed to create windows\n");
        free(copy1);
        free(copy2);
        return;
    }
    stbid_SetScaleMode(win1, STBID_SCALE_CENTER);
    stbid_SetScaleMode(win2, STBID_SCALE_STRETCH);

    printf("Total windows: %d\n", stbid_GetWindowCount());
    printf("Press 'f' bring Window 1 front, 'b' for Window 2, "
           "any other key changes title of Win1, ESC exit.\n");

    int counter = 0;
    int running = 1;
    while (running) {
        if (stbid_PollEvents()) break;
        int key = stbid_WaitKey(50);
        if (key != -1) {
            switch (key) {
                case 'f': stbid_BringToFront(win1); break;
                case 'b': stbid_BringToFront(win2); break;
                case 27:  running = 0; break;
                default: {
                    char title[64];
                    sprintf(title, "Window 1 #%d", ++counter);
                    stbid_SetWindowTitle(win1, title);
                    break;
                }
            }
        }
        Sleep(10);
    }
    stbid_CloseWindow(win1);
    stbid_CloseWindow(win2);
    free(copy1);
    free(copy2);
}

/* ---- Windows?only: screen capture ---- */
#ifdef _WIN32
static void example_screenshot(void) {
    printf("\n--- Screen Capture (Windows only) ---\n");
    HDC hdc = GetDC(NULL);
    unsigned char* img = NULL;
    int w, h, channels;
    if (stbid_CopyFromHDC(hdc, &img, &w, &h, &channels) == 0) {
        printf("Captured %dx%d\n", w, h);
        stbid_ShowImage(img, w, h, channels, "Screenshot (close to continue)");
        free(img);
    } else {
        fprintf(stderr, "CopyFromHDC error: %d\n", stbid_GetLastError());
    }
    ReleaseDC(NULL, hdc);
}
#endif

/* ---- Main ---- */
int main(int argc, char* argv[]) {
    if (stbid_Startup() != 0) {
        fprintf(stderr, "Startup error: %d\n", stbid_GetLastError());
        return 1;
    }

    int w, h, channels;
    unsigned char* img = load_image("input.jpg", &w, &h, &channels);
    if (!img) {
        stbid_Shutdown();
        return 1;
    }

    example_basic(img, w, h, channels);
    example_scaling_and_key(img, w, h, channels);
    example_multi_window(img, w, h, channels);

#ifdef _WIN32
    example_screenshot();
#endif

    stbi_image_free(img);
    stbid_Shutdown();

    printf("\nAll examples done. Press Enter to exit.\n");
    getchar();
    return 0;
}
