/**
 * stb_edit_example.c - Comprehensive test for stb_image_edit.h v1.3.0
 *
 * This example demonstrates:
 *   - Color conversion (RGB ? Grayscale, YUV, HSV)
 *   - Filtering (Gaussian, median, mean)
 *   - Edge detection (Sobel, Canny manual, Canny with adaptive Otsu)
 *   - Geometric transform (rotation)
 *   - Crop
 *   - Morphological operations (dilation, erosion)
 *   - Histogram equalization
 *   - Histogram calculation
 *
 * Compile (Linux/macOS):
 *   gcc -std=c99 -O2 stb_edit_example.c -lm -o example
 *
 * Compile (Windows/MSVC):
 *   cl /std:c99 /O2 stb_edit_example.c
 *
 * Run: ./example
 * Input: input.jpg (RGB image)
 * Output: multiple PNG files showing each operation result
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_EDIT_IMPLEMENT
#include "stb_image_edit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper: save a PNG and report success/failure */
static void save_png(const char *filename, stbi_uc *data, int w, int h, int channels) {
    if (stbi_write_png(filename, w, h, channels, data, w * channels)) {
        printf("  Saved: %s\n", filename);
    } else {
        fprintf(stderr, "  Failed to save: %s\n", filename);
    }
}

/* Helper: check result and print error if any */
static void check_result(int success, const char *operation) {
    if (!success) {
        stbie_error err = stbie_get_last_error();
        fprintf(stderr, "  %s failed: %s\n", operation, stbie_get_error_string(err));
    } else {
        printf("  %s succeeded\n", operation);
    }
}

int main(void) {
    int w, h, channels;
    // Force load as 3-channel RGB
    stbi_uc *rgb = stbi_load("input.jpg", &w, &h, &channels, 3);
    if (!rgb) {
        fprintf(stderr, "Failed to load input.jpg\n");
        return 1;
    }
    printf("Loaded: %d x %d, 3 channels (RGB)\n", w, h);

    /* ------------------------------------------------------------
       1. Color Space Conversions
       ------------------------------------------------------------ */
    // RGB -> Grayscale
    stbi_uc *gray = (stbi_uc*)malloc(w * h);
    if (gray) {
        int ok = stbie_cvt_color(rgb, w, h, 3, gray, 1, STBIE_COLOR_RGB2GRAY);
        check_result(ok, "RGB2GRAY");
        if (ok) save_png("gray.png", gray, w, h, 1);
    }

    // RGB -> YUV
    stbi_uc *yuv = (stbi_uc*)malloc(w * h * 3);
    if (yuv) {
        int ok = stbie_cvt_color(rgb, w, h, 3, yuv, 3, STBIE_COLOR_RGB2YUV);
        check_result(ok, "RGB2YUV");
        if (ok) save_png("yuv.png", yuv, w, h, 3);
        free(yuv);
    }

    // RGB -> HSV
    stbi_uc *hsv = (stbi_uc*)malloc(w * h * 3);
    if (hsv) {
        int ok = stbie_cvt_color(rgb, w, h, 3, hsv, 3, STBIE_COLOR_RGB2HSV);
        check_result(ok, "RGB2HSV");
        if (ok) save_png("hsv.png", hsv, w, h, 3);
        free(hsv);
    }

    // RGB -> BGR
    stbi_uc *bgr = (stbi_uc*)malloc(w * h * 3);
    if (bgr) {
        int ok = stbie_cvt_color(rgb, w, h, 3, bgr, 3, STBIE_COLOR_RGB2BGR);
        check_result(ok, "RGB2BGR");
        if (ok) save_png("bgr.png", bgr, w, h, 3);
        free(bgr);
    }

    /* ------------------------------------------------------------
       2. Filtering (on RGB)
       ------------------------------------------------------------ */
    // Mean blur (ksize=5)
    stbi_uc *blur_out = (stbi_uc*)malloc(w * h * 3);
    if (blur_out) {
        int ok = stbie_blur(rgb, w, h, 3, blur_out, 5, STBIE_BORDER_REPLICATE);
        check_result(ok, "mean blur");
        if (ok) save_png("blur.png", blur_out, w, h, 3);
        free(blur_out);
    }

    // Gaussian blur (sigma=1.5, ksize=5)
    stbi_uc *gauss_out = (stbi_uc*)malloc(w * h * 3);
    if (gauss_out) {
        int ok = stbie_blur_gaussian(rgb, w, h, 3, gauss_out, 1.5f, 5, STBIE_BORDER_REFLECT);
        check_result(ok, "gaussian blur");
        if (ok) save_png("gaussian.png", gauss_out, w, h, 3);
        free(gauss_out);
    }

    // Median blur (ksize=3)
    stbi_uc *median_out = (stbi_uc*)malloc(w * h * 3);
    if (median_out) {
        int ok = stbie_median_blur(rgb, w, h, 3, median_out, 3, STBIE_BORDER_ZERO);
        check_result(ok, "median blur");
        if (ok) save_png("median.png", median_out, w, h, 3);
        free(median_out);
    }

    /* ------------------------------------------------------------
       3. Morphological Operations (on grayscale)
       ------------------------------------------------------------ */
    if (gray) {
        // Dilation
        stbi_uc *dilated = (stbi_uc*)malloc(w * h);
        if (dilated) {
            int ok = stbie_dilate(gray, w, h, 1, dilated, 3, STBIE_BORDER_REPLICATE);
            check_result(ok, "dilation");
            if (ok) save_png("dilated.png", dilated, w, h, 1);
            free(dilated);
        }

        // Erosion
        stbi_uc *eroded = (stbi_uc*)malloc(w * h);
        if (eroded) {
            int ok = stbie_erode(gray, w, h, 1, eroded, 3, STBIE_BORDER_REPLICATE);
            check_result(ok, "erosion");
            if (ok) save_png("eroded.png", eroded, w, h, 1);
            free(eroded);
        }
    }

    /* ------------------------------------------------------------
       4. Histogram Calculation and Equalization
       ------------------------------------------------------------ */
    if (gray) {
        // Calculate histogram for grayscale
        int hist[1][256];
        if (stbie_histogram_calc(gray, w, h, 1, hist, 1)) {
            printf("  Histogram calc succeeded\n");
            // Print some statistics (e.g., peak value)
            int max_count = 0, max_gray = 0;
            for (int i = 0; i < 256; ++i) {
                if (hist[0][i] > max_count) {
                    max_count = hist[0][i];
                    max_gray = i;
                }
            }
            printf("    Most frequent gray level: %d (count %d)\n", max_gray, max_count);
        } else {
            stbie_error err = stbie_get_last_error();
            fprintf(stderr, "  Histogram calc failed: %s\n", stbie_get_error_string(err));
        }
    }

    // Histogram equalization on grayscale
    if (gray) {
        stbi_uc *eq_gray = (stbi_uc*)malloc(w * h);
        if (eq_gray) {
            int ok = stbie_histogram_equalize(gray, w, h, 1, eq_gray);
            check_result(ok, "histogram equalize (grayscale)");
            if (ok) save_png("eq_gray.png", eq_gray, w, h, 1);
            free(eq_gray);
        }
    }

    // Histogram equalization on color image
    stbi_uc *eq_color = (stbi_uc*)malloc(w * h * 3);
    if (eq_color) {
        int ok = stbie_histogram_equalize(rgb, w, h, 3, eq_color);
        check_result(ok, "histogram equalize (color)");
        if (ok) save_png("eq_color.png", eq_color, w, h, 3);
        free(eq_color);
    }

    /* ------------------------------------------------------------
       5. Edge Detection
       ------------------------------------------------------------ */
    // Sobel (on grayscale if available, otherwise on RGB)
    stbi_uc *sobel_out = (stbi_uc*)malloc(w * h);
    if (sobel_out) {
        int ok = stbie_sobel(rgb, w, h, 3, sobel_out, STBIE_BORDER_REPLICATE);
        check_result(ok, "Sobel");
        if (ok) save_png("sobel.png", sobel_out, w, h, 1);
        free(sobel_out);
    }

    // Canny manual thresholds
    stbi_uc *canny_out = (stbi_uc*)malloc(w * h);
    if (canny_out) {
        int ok = stbie_canny(rgb, w, h, 3, canny_out, 50, 150, 1.0f, 3, STBIE_BORDER_REFLECT);
        check_result(ok, "Canny manual");
        if (ok) save_png("canny.png", canny_out, w, h, 1);
        free(canny_out);
    }

    // Canny with adaptive Otsu
    stbi_uc *canny_auto = (stbi_uc*)malloc(w * h);
    if (canny_auto) {
        int ok = stbie_canny_with_adaptive_otsu(rgb, w, h, 3, canny_auto,
                                                 1.8f, 5, STBIE_BORDER_REFLECT,
                                                 64, 0.4f);
        check_result(ok, "Canny adaptive Otsu");
        if (ok) save_png("canny_auto.png", canny_auto, w, h, 1);
        free(canny_auto);
    }

    /* ------------------------------------------------------------
       6. Geometric Transform: Rotation
       ------------------------------------------------------------ */
    // Rotation 45° (crop mode)
    stbi_uc *rotate_crop = (stbi_uc*)malloc(w * h * 3);
    if (rotate_crop) {
        int out_w = w, out_h = h;
        int ok = stbie_rotate(rgb, w, h, 3, rotate_crop, &out_w, &out_h, 45.0f, STBIE_ROTATE_CROP);
        check_result(ok, "rotate (crop)");
        if (ok) save_png("rotate_crop.png", rotate_crop, out_w, out_h, 3);
        free(rotate_crop);
    }

    // Rotation 30° (expand mode)
    int exp_w, exp_h;
    stbie_rotate_calc_size(w, h, 30.0f, &exp_w, &exp_h);
    stbi_uc *rotate_expand = (stbi_uc*)malloc(exp_w * exp_h * 3);
    if (rotate_expand) {
        int ok = stbie_rotate(rgb, w, h, 3, rotate_expand, &exp_w, &exp_h, 30.0f, STBIE_ROTATE_EXPAND);
        check_result(ok, "rotate (expand)");
        if (ok) save_png("rotate_expand.png", rotate_expand, exp_w, exp_h, 3);
        free(rotate_expand);
    }

    /* ------------------------------------------------------------
       7. Crop
       ------------------------------------------------------------ */
    // Crop center 100x100 area
    if (w >= 100 && h >= 100) {
        int crop_x = w/2 - 50, crop_y = h/2 - 50;
        stbi_uc *crop_out = (stbi_uc*)malloc(100 * 100 * 3);
        if (crop_out) {
            int ok = stbie_crop(rgb, w, h, 3, crop_x, crop_y, 100, 100, crop_out);
            check_result(ok, "crop");
            if (ok) save_png("crop.png", crop_out, 100, 100, 3);
            free(crop_out);
        }
    }

    /* ------------------------------------------------------------
       8. Pixel Access Utilities (get/set)
       ------------------------------------------------------------ */
    stbi_uc *modified = (stbi_uc*)malloc(w * h * 3);
    if (modified) {
        memcpy(modified, rgb, w * h * 3);
        stbi_uc red[3] = {255, 0, 0};
        stbi_uc pixel[3];
        // Draw a 10x10 red square at top-left corner
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                if (stbie_get_pixel(modified, w, h, 3, x, y, pixel)) {
                    stbie_set_pixel(modified, w, h, 3, x, y, red);
                }
            }
        }
        save_png("modified.png", modified, w, h, 3);
        free(modified);
    }

    /* ------------------------------------------------------------
       9. Error Handling Demo (deliberate error)
       ------------------------------------------------------------ */
    stbi_uc *dummy = (stbi_uc*)malloc(10);
    int ok = stbie_cvt_color(rgb, w, h, 3, dummy, 2, STBIE_COLOR_RGB2GRAY);
    if (!ok) {
        stbie_error err = stbie_get_last_error();
        printf("  Expected error (channel mismatch): %s\n", stbie_get_error_string(err));
    }
    free(dummy);

    /* Cleanup */
    free(gray);
    stbi_image_free(rgb);

    printf("All operations completed.\n");
    return 0;
}
