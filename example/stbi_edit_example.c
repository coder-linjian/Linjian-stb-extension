/**
 * stbi_edit_example.c - 演示 stb_image_edit.h 的所有 API
 *
 * 输入: input.jpg (任意 RGB 图像)
 * 输出: 多个 PNG 文件，展示不同处理效果
 *
 * 编译: gcc -std=c99 -O2 stbi_edit_example.c -lm -o example
 * 运行: ./example// 
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

/* 辅助函数：安全保存图像，若失败则打印错误 */
static void save_png(const char *filename, stbi_uc *data, int w, int h, int channels) {
    if (stbi_write_png(filename, w, h, channels, data, w * channels)) {
        printf("  Saved: %s\n", filename);
    } else {
        fprintf(stderr, "  Failed to save: %s\n", filename);
    }
}

/* 辅助函数：检查转换结果，打印错误 */
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
    // 强制加载为 3 通道 RGB
    stbi_uc *rgb = stbi_load("input.jpg", &w, &h, &channels, 3);
    if (!rgb) {
        fprintf(stderr, "Failed to load input.jpg\n");
        return 1;
    }
    printf("Loaded: %d x %d, 3 channels (RGB)\n", w, h);

    /* ------------------------------------------------
       1. 色彩空间转换
       ------------------------------------------------ */

    // 1a. RGB -> 灰度
    stbi_uc *gray = (stbi_uc*)malloc(w * h);
    if (gray) {
        int ok = stbie_cvt_color(rgb, w, h, 3, gray, 1, STBIE_COLOR_RGB2GRAY);
        check_result(ok, "RGB2GRAY");
        if (ok) save_png("gray.png", gray, w, h, 1);
        free(gray);
    }

    // 1b. RGB -> YUV
    stbi_uc *yuv = (stbi_uc*)malloc(w * h * 3);
    if (yuv) {
        int ok = stbie_cvt_color(rgb, w, h, 3, yuv, 3, STBIE_COLOR_RGB2YUV);
        check_result(ok, "RGB2YUV");
        if (ok) save_png("yuv.png", yuv, w, h, 3);
        free(yuv);
    }

    // 1c. RGB -> HSV
    stbi_uc *hsv = (stbi_uc*)malloc(w * h * 3);
    if (hsv) {
        int ok = stbie_cvt_color(rgb, w, h, 3, hsv, 3, STBIE_COLOR_RGB2HSV);
        check_result(ok, "RGB2HSV");
        if (ok) save_png("hsv.png", hsv, w, h, 3);
        free(hsv);
    }

    // 1d. RGB -> BGR
    stbi_uc *bgr = (stbi_uc*)malloc(w * h * 3);
    if (bgr) {
        int ok = stbie_cvt_color(rgb, w, h, 3, bgr, 3, STBIE_COLOR_RGB2BGR);
        check_result(ok, "RGB2BGR");
        if (ok) save_png("bgr.png", bgr, w, h, 3);
        free(bgr);
    }

    /* ------------------------------------------------
       2. 滤波 (使用 RGB 原图)
       ------------------------------------------------ */

    // 2a. 均值模糊 (blur) ksize=5
    stbi_uc *blur_out = (stbi_uc*)malloc(w * h * 3);
    if (blur_out) {
        int ok = stbie_blur(rgb, w, h, 3, blur_out, 5, STBIE_BORDER_REPLICATE);
        check_result(ok, "blur");
        if (ok) save_png("blur.png", blur_out, w, h, 3);
        free(blur_out);
    }

    // 2b. 高斯模糊 sigma=1.5, ksize=5
    stbi_uc *gauss_out = (stbi_uc*)malloc(w * h * 3);
    if (gauss_out) {
        int ok = stbie_blur_gaussian(rgb, w, h, 3, gauss_out, 1.5f, 5, STBIE_BORDER_REFLECT);
        check_result(ok, "gaussian blur");
        if (ok) save_png("gaussian.png", gauss_out, w, h, 3);
        free(gauss_out);
    }

    // 2c. 中值滤波 ksize=3
    stbi_uc *median_out = (stbi_uc*)malloc(w * h * 3);
    if (median_out) {
        int ok = stbie_median_blur(rgb, w, h, 3, median_out, 3, STBIE_BORDER_ZERO);
        check_result(ok, "median blur");
        if (ok) save_png("median.png", median_out, w, h, 3);
        free(median_out);
    }

    /* ------------------------------------------------
       3. 边缘检测
       ------------------------------------------------ */

    // 3a. Sobel (自动转灰度，输出单通道)
    stbi_uc *sobel_out = (stbi_uc*)malloc(w * h);
    if (sobel_out) {
        int ok = stbie_sobel(rgb, w, h, 3, sobel_out, STBIE_BORDER_REPLICATE);
        check_result(ok, "Sobel");
        if (ok) save_png("sobel.png", sobel_out, w, h, 1);
        free(sobel_out);
    }

    // 3b. Canny (低阈值 50, 高阈值 150, sigma=1.0, ksize=3)
    stbi_uc *canny_gray = malloc(w * h);
	stbie_cvt_color(rgb, w, h, 3, canny_gray, 1, STBIE_COLOR_RGB2GRAY);
    stbi_uc *canny_out = (stbi_uc*)malloc(w * h);
    if (canny_out) {
        int ok = stbie_canny(canny_gray, w, h, 1, canny_out, 50, 150, 1.0f, 3, STBIE_BORDER_REFLECT);
        check_result(ok, "Canny");
        if (ok) save_png("canny.png", canny_out, w, h, 1);
        free(canny_out);
    }
    
    stbi_uc *edge = malloc(w * h);
	if (stbie_canny_with_adaptive_otsu(rgb, w, h, 3, edge,
                                    1.8f, 5, STBIE_BORDER_REFLECT,
                                    32, 0.4f)) {
    	stbi_write_png("canny_adaptive.png", w, h, 1, edge, w);
	}

    /* ------------------------------------------------
       4. 几何变换 (旋转)
       ------------------------------------------------ */

    // 4a. 旋转 45° (裁剪模式)
    stbi_uc *rotate_crop = (stbi_uc*)malloc(w * h * 3);
    if (rotate_crop) {
        int out_w = w, out_h = h;
        int ok = stbie_rotate(rgb, w, h, 3, rotate_crop, &out_w, &out_h, 45.0f, STBIE_ROTATE_CROP);
        check_result(ok, "rotate (crop)");
        if (ok) save_png("rotate_crop.png", rotate_crop, out_w, out_h, 3);
        free(rotate_crop);
    }

    // 4b. 旋转 30° (扩展模式)
    int exp_w, exp_h;
    stbie_rotate_calc_size(w, h, 30.0f, &exp_w, &exp_h);
    stbi_uc *rotate_expand = (stbi_uc*)malloc(exp_w * exp_h * 3);
    if (rotate_expand) {
        int ok = stbie_rotate(rgb, w, h, 3, rotate_expand, &exp_w, &exp_h, 30.0f, STBIE_ROTATE_EXPAND);
        check_result(ok, "rotate (expand)");
        if (ok) save_png("rotate_expand.png", rotate_expand, exp_w, exp_h, 3);
        free(rotate_expand);
    }

    /* ------------------------------------------------
       5. 裁剪
       ------------------------------------------------ */
    // 裁剪中心 100x100 区域
    int crop_x = w/2 - 50, crop_y = h/2 - 50;
    stbi_uc *crop_out = (stbi_uc*)malloc(100 * 100 * 3);
    if (crop_out) {
        int ok = stbie_crop(rgb, w, h, 3, crop_x, crop_y, 100, 100, crop_out);
        check_result(ok, "crop");
        if (ok) save_png("crop.png", crop_out, 100, 100, 3);
        free(crop_out);
    }

    /* ------------------------------------------------
       6. 像素访问 (get/set) – 在 RGB 原图上修改左上角 10x10 区域为红色
       ------------------------------------------------ */
    stbi_uc *modified = (stbi_uc*)malloc(w * h * 3);
    if (modified) {
        // 复制原图
        memcpy(modified, rgb, w * h * 3);

        stbi_uc red[3] = {255, 0, 0};
        stbi_uc pixel[3];
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                // 获取像素 (仅用于演示)
                if (stbie_get_pixel(modified, w, h, 3, x, y, pixel)) {
                    // 设置为红色
                    stbie_set_pixel(modified, w, h, 3, x, y, red);
                }
            }
        }
        save_png("modified.png", modified, w, h, 3);
        free(modified);
    }

    /* ------------------------------------------------
       7. 错误处理演示（故意调用错误参数）
       ------------------------------------------------ */
    stbi_uc *dummy = (stbi_uc*)malloc(10);
    int ok = stbie_cvt_color(rgb, w, h, 3, dummy, 2, STBIE_COLOR_RGB2GRAY);
    if (!ok) {
        stbie_error err = stbie_get_last_error();
        printf("  Expected error (channel mismatch): %s\n", stbie_get_error_string(err));
    }
    free(dummy);

    /* 释放原图 */
    stbi_image_free(rgb);
    printf("All operations completed.\n");
    return 0;
}
