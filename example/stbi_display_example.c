/*
 * stbi_display_simple.c - Minimal example using stb_image.h + stb_image_display.h
 *
 * Compile on Windows (MinGW):
 *   gcc stbi_display_simple.c -o simple.exe -lgdi32 -luser32
 * Compile on Linux:
 *   gcc stbi_display_simple.c -o simple -lX11
 *
 * Dependencies:
 *   - stb_image.h (separate file, available at https://github.com/nothings/stb)
 *   - stb_image_display.h (this library)
 *
 * To use, place "input.jpg" in the same directory as the executable.
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_DISPLAY_IMPLEMENTATION
#include "stb_image_display.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // 1. Initialize display subsystem
    if (stbid_Startup() != 0) {
        fprintf(stderr, "Failed to initialize display system, error: %d\n", stbid_GetLastError());
        return 1;
    }
    

    // 2. Load image from file using stb_image
    int width, height, channels;
    unsigned char* img = stbi_load("input.jpg", &width, &height, &channels, 3); // force RGB
    if (!img) {
        fprintf(stderr, "Failed to load 'input.jpg'\n");
        stbid_Shutdown();
        return 1;
    }

    printf("Loaded image: %dx%d, %d channels\n", width, height, channels);

    // 3. Show the image (blocking)
    
    printf("Displaying image. Close the window to continue.\n");
    int ret = stbid_ShowImage(img, width, height, 3, "Image Viewer");
    if (ret != 0) {
        fprintf(stderr, "Failed to display image, error: %d\n", stbid_GetLastError());
    }

#ifdef _WIN32 
	printf("Testing HDC Draw In Windows.\n");
    stbid_DrawToHDC(GetDC(NULL),0,0,width,height,img,width,height,channels);
#endif

    // 4. Cleanup
    stbi_image_free(img);
    stbid_Shutdown();

    printf("Done.\n");
    return 0;
}
