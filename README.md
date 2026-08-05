# Linjian-stb-extension

**stb 风格的单头文件图像处理、绘制与显示工具集**  
*A collection of stb‑style single‑header libraries for image processing, painting, and display*

[![Version](https://img.shields.io/github/v/release/coder-linjian/Linjian-stb-extension)](https://github.com/coder-linjian/Linjian-stb-extension/releases)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C99](https://img.shields.io/badge/C-99-blue.svg)](https://en.wikipedia.org/wiki/C99)

---

## 📖 概述 / Overview

**Linjian-stb-extension** 是一组单头文件的 C 语言库，专为图像处理、绘制与显示设计。  
*Linjian-stb-extension is a set of single‑header C libraries for image processing, painting, and display.*

它延续了 stb 家族“单文件、零依赖”的哲学，为游戏开发、嵌入式系统或任何需要轻量级图像操作的项目提供即插即用的解决方案。  
*It follows the stb philosophy: “single file, zero dependencies” and provides plug‑and‑play solutions for game development, embedded systems, or any project that needs lightweight image manipulation.*

当前版本 **v1.2.0** 包含三个独立库：  
*Current version **v1.2.0** includes three independent libraries:*

- **`stb_image_edit.h`** – 图像处理（色彩转换、滤波、边缘检测、旋转、裁剪、自适应阈值）  
  *Image processing (color conversion, filtering, edge detection, rotation, cropping, adaptive thresholding)*

- **`stb_image_paint.h`** – 图像绘制（像素、线、矩形、圆、多边形、文本、泛洪填充）  
  *Image painting (pixel, line, rectangle, circle, polygon, text, flood fill)*

- **`stb_image_display.h`** – 图像显示（跨平台窗口、阻塞/非阻塞模式、Windows HDC 绘制与捕获）  
  *Image display (cross‑platform windowing, blocking/non‑blocking modes, Windows HDC drawing & capture)*

---

## ✨ 特性 / Features

### `stb_image_edit.h`（图像编辑 / Image Editing）

- **色彩转换**：RGB ↔ 灰度 / YUV / HSV / BGR  
  *Color Conversion: RGB ↔ Grayscale / YUV / HSV / BGR*

- **滤波**：高斯模糊、中值滤波、均值模糊、自定义卷积  
  *Filtering: Gaussian blur, Median blur, Mean blur, Custom convolution*

- **边缘检测**：Sobel 梯度幅值 + 完整 Canny 算法（非极大值抑制采用插值法，边缘连续性好）  
  *Edge Detection: Sobel gradient magnitude + Full Canny algorithm (NMS with interpolation for better edge continuity)*

- **自适应阈值（新增于 v1.2.0）**：分块 Otsu 阈值生成、自适应二值化、基于自适应 Otsu 的 Canny 自动阈值  
  *Adaptive Thresholding (new in v1.2.0): block‑wise Otsu threshold map, adaptive binarization, Canny with automatic threshold from adaptive Otsu*

- **几何变换**：双线性插值旋转（裁剪 / 扩展模式）  
  *Geometric Transform: Bilinear‑interpolated rotation (crop / expand modes)*

- **工具**：裁剪、安全像素访问、标准化错误码  
  *Utilities: Cropping, safe pixel access, standardized error codes*

---

### `stb_image_paint.h`（图像绘制 / Image Painting）

- **基本绘图**：像素、Bresenham 线段、中点圆、矩形（填充/边框）、多边形（填充/轮廓）  
  *Basic Drawing: Pixel, Bresenham line, midpoint circle, rectangle (fill/stroke), polygon (fill/stroke)*

- **文本渲染**：
  - 内置 8×8 点阵字体（ASCII 32~126）  
    *Built‑in 8×8 bitmap font (ASCII 32~126)*
  - **TrueType 支持（可选）**：定义 `STBIP_USE_TRUETYPE` 后，用户需额外包含 `stb_truetype.h`，即可使用 TrueType 字体绘制 UTF‑8 或 ANSI 编码文本。  
    *TrueType support (optional): define `STBIP_USE_TRUETYPE` and include `stb_truetype.h` to draw UTF‑8 or ANSI text with TrueType fonts.*

- **泛洪填充**：种子填充（4/8 邻接，可调容差）  
  *Flood Fill: Seed‑based fill (4/8 connectivity, adjustable tolerance)*

- **清除图像**：用指定颜色填充整个画布  
  *Clear Image: Fill entire canvas with specified color*

---

### `stb_image_display.h`（图像显示 / Image Display）

- **跨平台窗口**：支持 Windows（GDI）和 Linux（X11），macOS 明确拒绝编译  
  *Cross‑platform windowing: supports Windows (GDI) and Linux (X11), macOS explicitly rejected*

- **阻塞式显示**：`stbid_ShowImage` – 创建窗口并阻塞直至用户关闭  
  *Blocking display: `stbid_ShowImage` – creates a window and blocks until user closes it*

- **非阻塞式显示**：`stbid_ShowImageNonBlocking` / `stbid_UpdateImage` / `stbid_IsWindowOpen` / `stbid_CloseWindow` / `stbid_PollEvents` – 适用于视频流或实时预览  
  *Non‑blocking display: `stbid_ShowImageNonBlocking` / `stbid_UpdateImage` / `stbid_IsWindowOpen` / `stbid_CloseWindow` / `stbid_PollEvents` – ideal for video streams or live preview*

- **Windows 专用绘制与捕获接口**：`stbid_DrawToHDC` 和 `stbid_CopyFromHDC` – 支持绘制到任意设备上下文，以及从 HDC 捕获图像数据  
  *Windows‑only drawing & capture: `stbid_DrawToHDC` and `stbid_CopyFromHDC` – draw to any HDC and capture image data from HDC*

- **零依赖、极简集成**：仅使用 C 标准库和操作系统原生 API  
  *Zero dependencies, minimal integration: uses only the C standard library and native OS APIs*

---

## 🐞 v1.2.0 主要改进 / Major Improvements

- **新增自适应 Otsu 阈值处理函数**：`stbie_adaptive_otsu_threshold`、`stbie_adaptive_otsu_binarize` 和 `stbie_canny_with_adaptive_otsu`，可自动适应局部光照变化，显著提升复杂图像的二值化和边缘检测效果。  
  *Added adaptive Otsu thresholding functions: `stbie_adaptive_otsu_threshold`, `stbie_adaptive_otsu_binarize`, and `stbie_canny_with_adaptive_otsu` – automatically adapt to local illumination changes, significantly improving binarization and edge detection on complex images.*

- **Canny 边缘检测的阈值自动选择**：通过自适应 Otsu 计算高阈值，简化调参过程。  
  *Automatic threshold selection for Canny edge detection – high threshold computed from adaptive Otsu, simplifying parameter tuning.*

- **性能优化**：优化了内存分配策略，减少冗余 malloc/free 调用。  
  *Performance optimization: improved memory allocation strategy to reduce redundant malloc/free calls.*

---

## 🚀 快速开始 / Quick Start

### 使用 `stb_image_edit.h`（图像编辑 / Image Editing）
```c
#define STB_IMAGE_EDIT_IMPLEMENT
#include "stb_image_edit.h"

// 将 RGB 图像转为灰度
stbie_cvt_color(rgb, w, h, 3, gray, 1, STBIE_COLOR_RGB2GRAY);

// 使用自适应 Otsu 进行二值化
stbi_uc *binary = malloc(w * h);
stbie_adaptive_otsu_binarize(gray, w, h, 64, binary);

// 使用自适应 Otsu 自动阈值进行 Canny 边缘检测
stbi_uc *edge = malloc(w * h);
stbie_canny_with_adaptive_otsu(rgb, w, h, 3, edge,
1.8f, 5, STBIE_BORDER_REFLECT,
64, 0.4f);
```


### 使用 `stb_image_paint.h`（图像绘制 / Image Painting）

```c
#define STB_IMAGE_PAINT_IMPLEMENT
#include "stb_image_paint.h"

// 在 RGB 图像上画一条红色对角线
stbi_uc red[3] = {255, 0, 0};
stbip_draw_line(image, w, h, 3, 0, 0, w-1, h-1, red);
```


### 使用 `stb_image_display.h`（图像显示 / Image Display）
```c
#define STB_IMAGE_DISPLAY_IMPLEMENT
#include "stb_image_display.h"

// 阻塞式显示
if (stbid_Startup() == 0) {
stbid_ShowImage(rgb, w, h, 3, "My Image");
stbid_Shutdown();
}
```


---

## 📚 API 概览 / API Overview

### `stbie_*`（编辑库 / Editing Library）

#### 基本图像处理函数

| 函数 | 描述（中文） | 描述（英文） |
|------|-------------|-------------|
| `stbie_cvt_color` | 色彩空间转换（RGB↔GRAY/YUV/HSV/BGR） | Color space conversion |
| `stbie_filter_2d` | 自定义核二维卷积 | 2D convolution with custom kernel |
| `stbie_blur_gaussian` | 高斯模糊（可分离） | Gaussian blur (separable) |
| `stbie_median_blur` | 中值滤波 | Median filter |
| `stbie_blur` | 均值模糊（盒式滤波） | Mean blur (box filter) |
| `stbie_sobel` | Sobel 梯度幅值 | Sobel gradient magnitude |
| `stbie_canny` | Canny 边缘检测（手动阈值） | Canny edge detection (manual thresholds) |
| `stbie_rotate` | 图像旋转（双线性插值） | Image rotation (bilinear) |
| `stbie_crop` | 图像裁剪 | Image cropping |

#### 自适应阈值函数（v1.2.0 新增）

| 函数 | 描述（中文） | 描述（英文） |
|------|-------------|-------------|
| `stbie_adaptive_otsu_threshold` | 生成分块 Otsu 阈值图 | Generate block‑wise Otsu threshold map |
| `stbie_adaptive_otsu_binarize` | 自适应 Otsu 二值化 | Adaptive Otsu binarization |
| `stbie_canny_with_adaptive_otsu` | Canny 边缘检测（自动阈值来自自适应 Otsu） | Canny edge detection with auto‑threshold from adaptive Otsu |

---

### `stbip_*`（绘制库 / Painting Library）

所有绘制函数均自动裁剪到图像边界，并返回 1（成功）或 0（失败，可通过 `stbip_get_last_error` 获取错误码）。  
*All drawing functions automatically clip to image boundaries and return 1 on success or 0 on failure (error code available via `stbip_get_last_error`).*

| 函数 | 描述（中文） | 描述（英文） |
|------|-------------|-------------|
| `stbip_draw_pixel` | 绘制单个像素 | Draws a single pixel |
| `stbip_draw_line` | 使用 Bresenham 算法绘制线段 | Draws a line using Bresenham’s algorithm |
| `stbip_draw_rect` | 绘制矩形（填充或边框，可调线宽） | Draws a rectangle (filled or stroked, adjustable thickness) |
| `stbip_draw_circle` | 绘制圆（填充或轮廓，中点算法） | Draws a circle (filled or outlined, midpoint algorithm) |
| `stbip_draw_polygon` | 绘制多边形（顶点数组，填充或轮廓） | Draws a polygon (vertex array, filled or stroked) |
| `stbip_draw_text` | 使用内置 8×8 点阵字体绘制 ASCII 文本 | Draws ASCII text using built‑in 8×8 bitmap font |
| `stbip_flood_fill` | 从种子点泛洪填充（4/8 邻接，容差可调） | Flood fill from seed (4/8 connectivity, adjustable tolerance) |
| `stbip_clear_image` | 用指定颜色填充整个图像（别名 `stbip_clear`） | Fills entire image with specified color (alias `stbip_clear`) |

**TrueType 字体支持（需定义 `STBIP_USE_TRUETYPE`）**  
*TrueType font support (requires `STBIP_USE_TRUETYPE`)*

| 函数 | 描述（中文） | 描述（英文） |
|------|-------------|-------------|
| `stbip_ttf_load` | 加载 TrueType 字体文件，返回字体索引 | Loads TrueType font, returns font index |
| `stbip_ttf_set_current` | 设置当前字体 | Sets current font |
| `stbip_draw_text_ttf_utf8` | 绘制 UTF‑8 文本 | Draws UTF‑8 text |
| `stbip_draw_text_ttf_ansi` | 绘制系统 ANSI 编码文本 | Draws system ANSI encoded text |
| `stbip_draw_text_ttf` | 宏别名，等价于 `stbip_draw_text_ttf_ansi` | Macro alias for `stbip_draw_text_ttf_ansi` |
| `stbip_ttf_done` | 释放所有已加载的 TrueType 字体资源 | Frees all loaded TrueType fonts |

---

### `stbid_*`（显示库 / Display Library）

| 函数 | 描述（中文） | 描述（英文） |
|------|-------------|-------------|
| `stbid_Startup` | 初始化窗口子系统 | Initializes windowing subsystem |
| `stbid_Shutdown` | 释放所有资源 | Releases all resources |
| `stbid_ShowImage` | 阻塞式显示图像（阻塞至关闭） | Blocking image display (blocks until closed) |
| `stbid_ShowImageNonBlocking` | 非阻塞式创建窗口，返回句柄 | Non‑blocking window creation, returns handle |
| `stbid_UpdateImage` | 更新窗口图像数据 | Updates window image data |
| `stbid_IsWindowOpen` | 检查窗口是否仍打开 | Checks if window is still open |
| `stbid_CloseWindow` | 关闭窗口 | Closes window |
| `stbid_PollEvents` | 轮询事件，返回退出标志 | Polls events, returns quit flag |
| `stbid_DrawToHDC` | （Windows）绘制到 HDC | (Windows) Draw to HDC |
| `stbid_CopyFromHDC` | （Windows）从 HDC 捕获图像数据 | (Windows) Capture image data from HDC |
| `stbid_GetLastError` | 获取最后一次错误码 | Gets last error code |

错误码包括：`STBID_SUCCESS`、`STBID_ERR_PARAM`、`STBID_ERR_NOT_INIT`、`STBID_ERR_NO_MEM`、`STBID_ERR_WINDOW`、`STBID_ERR_HDC`、`STBID_ERR_PLATFORM_STUB_FUNC`。  
*Error codes include: `STBID_SUCCESS`, `STBID_ERR_PARAM`, `STBID_ERR_NOT_INIT`, `STBID_ERR_NO_MEM`, `STBID_ERR_WINDOW`, `STBID_ERR_HDC`, `STBID_ERR_PLATFORM_STUB_FUNC`.*

---

## 🛠️ 集成方法 / Integration

1. 下载所需的 `.h` 文件放入项目目录。  
   *Download the required `.h` files and place them in your project directory.*

2. 在 **且仅在** 一个 `.c` 文件中定义实现宏并包含头文件：  
   *In **exactly one** `.c` file, define the implementation macro and include the header:*
```c
#define STB_IMAGE_EDIT_IMPLEMENT // 编辑库
#include "stb_image_edit.h"

#define STB_IMAGE_PAINT_IMPLEMENT // 绘制库
#include "stb_image_paint.h"

#define STB_IMAGE_DISPLAY_IMPLEMENT // 显示库
#include "stb_image_display.h"
```

3. 如需 TrueType 支持，额外包含 `stb_truetype.h` 并定义 `STBIP_USE_TRUETYPE`。  
*For TrueType support, additionally include `stb_truetype.h` and define `STBIP_USE_TRUETYPE`.*

4. 编译时链接对应平台的库：  
*Link the appropriate libraries when compiling:*

- **Windows (MinGW/MSVC)**：`-lgdi32 -luser32`
- **Linux**：`-lX11`
- **macOS**：不支持 (因为Objective-C) / Not supported (Because of Obiective-C)

5. 始终链接数学库 `-lm`。  
*Always link the math library `-lm`.*

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

- **v1.3.0**：形态学操作（膨胀、腐蚀）、直方图均衡化  
- **v1.4.0**：仿射变换（缩放、平移、剪切）、图像金字塔  
- **未来**：可选 SIMD 加速

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
