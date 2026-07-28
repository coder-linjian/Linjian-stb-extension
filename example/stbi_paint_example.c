/*
 * stbi_paint_example.c - 使用 stb_image_paint.h 绘制图形并保存为 PNG
 *
 * 编译（启用 TrueType）：
 *   gcc -std=c99 -O2 stbi_paint_example.c -lm -o stbi_paint_example \
 *       -DSTB_IMAGE_WRITE_IMPLEMENTATION \
 *       -DSTB_IMAGE_PAINT_IMPLEMENTATION \
 *       -DSTBIP_USE_TRUETYPE
 *
 * 编译（不启用 TrueType，仅使用内置位图字体）：
 *   gcc -std=c99 -O2 stbi_paint_example.c -lm -o stbi_paint_example \
 *       -DSTB_IMAGE_WRITE_IMPLEMENTATION \
 *       -DSTB_IMAGE_PAINT_IMPLEMENTATION
 *
 * 运行：
 *   ./stbi_paint_example
 *
 * 依赖：
 *   - stb_image_write.h（用于保存 PNG）
 *   - stb_image_paint.h（绘图库）
 *   - stb_truetype.h（可选，若启用 TrueType）
 *
 * 注意：
 *   - 启用 TrueType 时需要准备一个 TrueType 字体文件（如 arial.ttf），
 *     并将其路径赋给 FONT_PATH 宏。
 *   - 代码中使用了中文字符串，若在 Windows 下，确保源文件保存为 GBK 编码，
 *     以使用 ANSI 版本；若为 UTF-8，则使用 stbip_draw_text_ttf_utf8()。
 *   - 本示例默认启用 TrueType，若没有字体文件，可定义 NO_TTF 来跳过。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * stbi_paint_example.c - 使用 stb_image_paint.h 绘制图形并保存为 PNG
 *
 * 编译（启用 TrueType）：
 *   gcc -std=c99 -O2 stbi_paint_example.c -lm -o stbi_paint_example \
 *       -DSTB_IMAGE_WRITE_IMPLEMENTATION \
 *       -DSTB_IMAGE_PAINT_IMPLEMENTATION \
 *       -DSTBIP_USE_TRUETYPE
 *
 * 编译（不启用 TrueType）：
 *   gcc -std=c99 -O2 stbi_paint_example.c -lm -o stbi_paint_example \
 *       -DSTB_IMAGE_WRITE_IMPLEMENTATION \
 *       -DSTB_IMAGE_PAINT_IMPLEMENTATION
 *
 * 运行：
 *   ./stbi_paint_example
 *
 * 依赖：
 *   - stb_image_write.h（用于保存 PNG）
 *   - stb_image_paint.h（绘图库）
 *   - stb_truetype.h（可选，若启用 TrueType，需在编译时定义 -DSTBIP_USE_TRUETYPE）
 *
 * 注意：
 *   - 启用 TrueType 时需要准备一个 TrueType 字体文件（如 arial.ttf），
 *     并将其路径赋给 FONT_PATH 宏。
 *   - 代码中使用了中文字符串，若在 Windows 下，确保源文件保存为 GBK 编码，
 *     以使用 ANSI 版本；若为 UTF-8，则使用 stbip_draw_text_ttf_utf8()。
 *   - 本示例默认通过命令行定义 STBIP_USE_TRUETYPE，若未定义则跳过 TTF 部分。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 必须在包含 stb_image_paint.h 之前定义 STBIP_USE_TRUETYPE */
#ifdef STBIP_USE_TRUETYPE
# include "stb_truetype.h"
#endif

/* 定义实现宏，必须在包含头文件之前 */
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_PAINT_IMPLEMENTATION
#include "stb_image_paint.h"

/* 字体路径（用户可根据实际情况修改） */
#define FONT_PATH "arial.ttf"

/* 图像尺寸 */
#define WIDTH  640
#define HEIGHT 480

int main(void) {
    int channels = 3; /* RGB */
    unsigned char *image = (unsigned char*)malloc(WIDTH * HEIGHT * channels);
    if (!image) {
        fprintf(stderr, "内存分配失败\n");
        return 1;
    }

    /* 1. 填充白色背景 */
    stbi_uc white[4] = {255, 255, 255, 255};
    stbip_clear_image(image, WIDTH, HEIGHT, channels, white);

    /* 2. 绘制红色矩形边框 */
    stbi_uc red[4] = {255, 0, 0, 255};
    stbip_draw_rect(image, WIDTH, HEIGHT, channels,
                    20, 20, 200, 150, red, 0, 3);

    /* 3. 绘制绿色填充矩形 */
    stbi_uc green[4] = {0, 255, 0, 255};
    stbip_draw_rect(image, WIDTH, HEIGHT, channels,
                    40, 40, 160, 110, green, 1, 1);

    /* 4. 绘制蓝色对角线 */
    stbi_uc blue[4] = {0, 0, 255, 255};
    stbip_draw_line(image, WIDTH, HEIGHT, channels,
                    0, 0, WIDTH-1, HEIGHT-1, blue);

    /* 5. 绘制青色圆形（填充） */
    stbi_uc cyan[4] = {0, 255, 255, 255};
    stbip_draw_circle(image, WIDTH, HEIGHT, channels,
                      500, 150, 80, cyan, 1);

    /* 6. 绘制紫色圆形轮廓 */
    stbi_uc purple[4] = {128, 0, 128, 255};
    stbip_draw_circle(image, WIDTH, HEIGHT, channels,
                      500, 150, 80, purple, 0);

    /* 7. 绘制多边形（五角星） */
    int star_verts[] = {
        320, 60,
        340, 120,
        400, 120,
        355, 160,
        370, 220,
        320, 185,
        270, 220,
        285, 160,
        240, 120,
        300, 120
    };
    stbi_uc orange[4] = {255, 165, 0, 255};
    stbip_draw_polygon(image, WIDTH, HEIGHT, channels,
                       star_verts, 10, orange, 0);

    /* 8. 使用内置位图字体绘制 ASCII 文本 */
    stbi_uc black[4] = {0, 0, 0, 255};
    stbip_draw_text(image, WIDTH, HEIGHT, channels,
                    30, 220, "Hello, stb_image_paint!", black);

#ifdef STBIP_USE_TRUETYPE
    /* 9. 加载 TrueType 字体 */
    int font_id = stbip_ttf_load(FONT_PATH);
    if (font_id < 0) {
        fprintf(stderr, "加载字体失败: %s (错误码 %d)\n",
                FONT_PATH, stbip_get_last_error());
    } else {
        /* 9a. 使用 ANSI 版本绘制中文（源文件需保存为 GBK 编码） */
        stbi_uc magenta[4] = {255, 0, 255, 255};
        /* 若您的源文件是 GBK 编码，直接使用 stbip_draw_text_ttf（默认ANSI） */
        stbip_draw_text_ttf(image, WIDTH, HEIGHT, channels,
                            30, 280, "你好，TrueType ANSI！", magenta, 32.0f);

        /* 9b. 使用 UTF-8 版本绘制中文（源文件保存为 UTF-8） */
        stbi_uc yellow[4] = {255, 255, 0, 255};
        /* 如果您的源文件是 UTF-8 编码，可直接调用 UTF-8 版本 */
        /* 这里为了演示，使用相同字符串（实际取决于源文件编码） */
        stbip_draw_text_ttf_utf8(image, WIDTH, HEIGHT, channels,
                                 30, 330, "UTF-8: 你好，世界！", yellow, 32.0f);

        /* 释放字体资源（可选，程序结束时会自动释放，但主动调用更规范） */
        stbip_ttf_done();
    }
#endif /* STBIP_USE_TRUETYPE */

    /* 10. 检查是否有错误 */
    stbip_error err = stbip_get_last_error();
    if (err != STBIP_ERROR_NONE) {
        fprintf(stderr, "绘制过程中发生错误: %d\n", err);
    }

    /* 11. 保存为 PNG */
    int success = stbi_write_png("output.png", WIDTH, HEIGHT, channels,
                                 image, WIDTH * channels);
    if (success) {
        printf("图像已保存为 output.png\n");
    } else {
        fprintf(stderr, "保存 PNG 失败\n");
    }

    free(image);
    return 0;
}
