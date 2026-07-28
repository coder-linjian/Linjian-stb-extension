# Linjian-stb-extension

**stb 风格的单头文件图像处理与绘制工具集**  
*A collection of stb‑style single‑header libraries for image processing and painting*

[![Version](https://img.shields.io/github/v/release/coder-linjian/Linjian-stb-extension)](https://github.com/coder-linjian/Linjian-stb-extension/releases)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C99](https://img.shields.io/badge/C-99-blue.svg)](https://en.wikipedia.org/wiki/C99)

---

## 📖 概述 / Overview

**Linjian-stb-extension** 是一组单头文件的 C 语言库，专为图像处理与绘制设计。  
*Linjian-stb-extension is a set of single‑header C libraries for image processing and drawing.*

它延续了 stb 家族“单文件、零依赖”的哲学，为游戏开发、嵌入式系统或任何需要轻量级图像操作的项目提供即插即用的解决方案。  
*It follows the stb philosophy: “single file, zero dependencies” and provides plug‑and‑play solutions for game development, embedded systems, or any project that needs lightweight image manipulation.*

当前版本 **v1.1.0** 包含两个独立库：  
*Current version **v1.1.0** includes two independent libraries:*

- **`stb_image_edit.h`** – 图像处理（色彩转换、滤波、边缘检测、旋转、裁剪）  
  *Image processing (color conversion, filtering, edge detection, rotation, cropping)*

- **`stb_image_paint.h`** – 图像绘制（像素、线、矩形、圆、多边形、文本、泛洪填充）  
  *Image painting (pixel, line, rectangle, circle, polygon, text, flood fill)*

---

## ✨ 特性 / Features

### `stb_image_edit.h`（图像编辑 / Image Editing）

- **色彩转换**：RGB ↔ 灰度 / YUV / HSV / BGR  
  *Color Conversion: RGB ↔ Grayscale / YUV / HSV / BGR*

- **滤波**：高斯模糊、中值滤波、均值模糊、自定义卷积  
  *Filtering: Gaussian blur, Median blur, Mean blur, Custom convolution*

- **边缘检测**：Sobel 梯度幅值 + 完整 Canny 算法（非极大值抑制采用插值法，边缘连续性好）  
  *Edge Detection: Sobel gradient magnitude + Full Canny algorithm (NMS with interpolation for better edge continuity)*

- **几何变换**：双线性插值旋转（裁剪 / 扩展模式）  
  *Geometric Transform: Bilinear‑interpolated rotation (crop / expand modes)*

- **工具**：裁剪、安全像素访问、标准化错误码  
  *Utilities: Cropping, safe pixel access, standardized error codes*

### `stb_image_paint.h`（图像绘制 / Image Painting）— 新增于 v1.1.0

- **基本绘图**：像素、Bresenham 线段、中点圆、矩形（填充/边框）  
  *Basic Drawing: Pixel, Bresenham line, midpoint circle, rectangle (fill/stroke)*

- **多边形绘制**：任意顶点数组，扫描线填充算法  
  *Polygon Drawing: Arbitrary vertex arrays with scanline fill algorithm*

- **文本渲染**：
  - 内置 8x8 位图字体（ASCII 32~126）  
    *Built‑in 8x8 bitmap font (ASCII 32~126)*
  - **TrueType 支持（可选）**：定义 `STBIP_USE_TRUETYPE` 后，用户需额外包含 `stb_truetype.h`，即可使用 TrueType 字体绘制简单文本（支持中英文等，但不支持复杂排版，如双向文本、连字、换行等）。  
    *TrueType support (optional): define `STBIP_USE_TRUETYPE` and include `stb_truetype.h` to draw simple text (supports Chinese, English, etc., but not complex layout like bidirectional, ligatures, or line wrapping).*

- **泛洪填充**：种子填充（4/8 邻接，可调容差）  
  *Flood Fill: Seed‑based fill (4/8 connectivity, adjustable tolerance)*

- **清除图像**：用指定颜色填充整个画布（`stbip_clear`）  
  *Clear Image: Fill entire canvas with specified color (`stbip_clear`)*

---

## 🐞 v1.1.0 Bug 修复 / Bug Fixes

- **修复 Canny 边缘检测碎片化问题**  
  将非极大值抑制（NMS）从离散四方向比较升级为**双线性插值法**，显著改善斜线/曲线边缘的连续性，大幅减少边缘断线和孤立点。  
  *Fixed Canny edge detection fragmentation – upgraded non‑maximum suppression (NMS) from discrete 4‑direction comparison to **bilinear interpolation**, significantly improving continuity on diagonal/curved edges and reducing broken lines and isolated points.*

---

## 🚀 快速开始 / Quick Start

### 使用 `stb_image_edit.h`（图像编辑 / Image Editing）

```c
#define STB_IMAGE_EDIT_IMPLEMENT
#include "stb_image_edit.h"

// 将 RGB 图像转为灰度
stbie_cvt_color(rgb, w, h, 3, gray, 1, STBIE_COLOR_RGB2GRAY);
```

### 使用 `stb_image_paint.h`（图像绘制 / Image Painting）

```c
#define STB_IMAGE_PAINT_IMPLEMENT
#include "stb_image_paint.h"

// 在 RGB 图像上画一条红色对角线
stbi_uc red[3] = {255, 0, 0};
stbip_draw_line(image, w, h, 3, 0, 0, w-1, h-1, red);
```

### 使用 TrueType 字体（可选 / Optional）

```c
#define STBIP_USE_TRUETYPE
#define STB_IMAGE_PAINT_IMPLEMENT
#include "stb_image_paint.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h" // 用户自行提供

// 加载字体，加载后自动设为当前字体
int font_id = stbip_ttf_load("arial.ttf");
if (font_id >= 0) {
stbi_uc white[4] = {255, 255, 255, 255};
// 绘制英文
stbip_draw_text_ttf(image, w, h, 3, 10, 10, "Hello", white, 24.0f);
// 绘制中文（注意：字体文件需包含中文字符）
stbip_draw_text_ttf(image, w, h, 3, 10, 40, "你好", white, 24.0f);
} else {
fprintf(stderr, "Load failed: %d\n", stbip_get_last_error());
}

// 若需要切换字体（假设加载了第二个字体）
int font_id2 = stbip_ttf_load("times.ttf");
stbip_ttf_set_current(font_id2); // 切换到第二字体

// 释放所有已加载字体（无需逐个释放）
stbip_ttf_done();
```


---

## 📚 API 概览 / API Overview

### `stbie_*`（编辑库 / Editing Library）

详见 `stb_image_edit.h` 文件头注释。  
*See comments at the top of `stb_image_edit.h` for details.*

### `stbip_*`（绘制库 / Painting Library）

| 函数 | 说明 |
|------|------|
| `stbip_draw_pixel` | 设置单个像素颜色 |
| `stbip_draw_line` | Bresenham 线段 |
| `stbip_draw_rect` | 矩形（填充/边框，线宽可调） |
| `stbip_draw_circle` | 圆（填充/边框，中点算法） |
| `stbip_draw_polygon` | 多边形（顶点数组，填充/边框） |
| `stbip_draw_text` | ASCII 文本（内置位图字体） |
| `stbip_flood_fill` | 泛洪填充（4/8 邻接） |
| `stbip_clear` | 用颜色填充整幅图像 |
| **TrueType 支持（条件编译）** | |
| `stbip_ttf_load` | 加载 TrueType 字体文件，返回字体 ID（加载后自动设为当前字体） |
| `stbip_ttf_set_current` | 设置当前使用的 TrueType 字体上下文 |
| `stbip_draw_text_ttf` | 使用 TrueType 字体绘制简单文本（支持中英文等，不支持复杂排版） |
| `stbip_ttf_done` | 释放所有已加载的 TrueType 字体资源 |

---

## 🛠️ 集成方法 / Integration

1. 下载所需的 `.h` 文件放入项目目录。  
   *Download the required `.h` files and place them in your project directory.*

2. 在 **且仅在** 一个 `.c` 文件中定义实现宏并包含头文件：  
   *In **exactly one** `.c` file, define the implementation macro and include the header:*

```c
#define STB_IMAGE_EDIT_IMPLEMENT // 或 STB_IMAGE_PAINT_IMPLEMENT
#include "stb_image_edit.h" // 或 "stb_image_paint.h"
```

3. 如需 TrueType 支持，请额外包含 `stb_truetype.h` 并定义 `STBIP_USE_TRUETYPE`。  
*For TrueType support, additionally include `stb_truetype.h` and define `STBIP_USE_TRUETYPE`.*

4. 编译时链接数学库 `-lm`。  
*Link with the math library `-lm` when compiling.*

---

## 📄 许可证 / License

MIT – 自由使用，含商业项目。详见 [LICENSE](LICENSE) 文件。  
*MIT – Free for any use, including commercial projects. See [LICENSE](LICENSE) for details.*

---

## 🙏 致谢 / Credits

- 基于 [Sean Barrett](https://github.com/nothings) 的 `stb` 项目（公共领域）而构建。  
*Built on the shoulders of Sean Barrett's `stb` project (public domain).*

- 灵感源自 OpenCV。  
*Inspired by OpenCV.*

- 由 **Linjian Studio**（[@coder-linjian](https://github.com/coder-linjian)）开发维护。  
*Developed and maintained by **Linjian Studio** ([@coder-linjian](https://github.com/coder-linjian)).*

---

## 🗺️ 路线图 / Roadmap

- **v1.2.0**：Alpha 混合、抗锯齿线、椭圆绘制  
- **v1.3.0**：仿射变换、图像金字塔  
- **未来**：可选 SIMD 加速（通过编译宏）

---

## 🤝 贡献 / Contributing

欢迎提交 Issue 和 Pull Request。请确保代码风格一致并附上适当注释。  
*Issues and pull requests are welcome! Please ensure your code follows the existing style and includes appropriate comments.*

---

## 📬 联系方式 / Contact

如有疑问或建议，请在 GitHub 上提交 Issue。  
*For questions or suggestions, please open an issue on GitHub.*

---

**祝您图像处理愉快！** 🖼️  
*Happy image processing!* 🖼️


