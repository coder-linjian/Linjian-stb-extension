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

当前版本 **v1.3.0** 包含三个独立库：  
*Current version **v1.3.0** includes three independent libraries:*

- **`stb_image_edit.h`** – 图像处理（色彩转换、滤波、边缘检测、旋转、裁剪、形态学、直方图均衡）  
  *Image processing (color conversion, filtering, edge detection, rotation, cropping, morphology, histogram equalization)*

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

- **自适应阈值（v1.2.0+）**：分块 Otsu 阈值生成、自适应二值化、基于自适应 Otsu 的 Canny 自动阈值  
  *Adaptive Thresholding (v1.2.0+): block‑wise Otsu threshold map, adaptive binarization, Canny with auto‑threshold from adaptive Otsu*

- **形态学操作（v1.3.0）**：膨胀、腐蚀（矩形核）  
  *Morphological Operations (v1.3.0): dilation, erosion (rectangular kernel)*

- **直方图处理（v1.3.0）**：直方图均衡化（支持灰度/彩色）、直方图计算  
  *Histogram Processing (v1.3.0): histogram equalization (grayscale/color), histogram calculation*

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
  - **v1.1.1 新增**：若用户希望手动控制 `stb_truetype.h` 的包含顺序，可在包含本库前定义 `STBIP_NOT_AUTO_INCLUDE_TRUETYPE`，禁用自动包含，改为手动包含 `stb_truetype.h`，避免多文件项目中重复定义问题。  
    *New in v1.1.1: define `STBIP_NOT_AUTO_INCLUDE_TRUETYPE` before including this header to disable automatic inclusion of `stb_truetype.h` and include it manually, avoiding multiple definition issues in multi‑file projects.*

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

- **高质量缩放（v1.1.1+）**：Windows 下启用双三次插值，图像缩放平滑无锯齿  
  *High‑quality scaling (v1.1.1+): bicubic interpolation on Windows, smooth and anti‑aliased scaling*

- **三种缩放模式**：  
  - `STBID_SCALE_STRETCH` – 拉伸填满窗口（可能变形）  
  - `STBID_SCALE_KEEP_ASPECT` – 保持宽高比，完整显示（留黑边）  
  - `STBID_SCALE_CENTER` – 原始尺寸居中（超出裁剪）  
  *Three scaling modes: `STBID_SCALE_STRETCH` (stretch, may distort), `STBID_SCALE_KEEP_ASPECT` (preserve aspect, black bars), `STBID_SCALE_CENTER` (original size, cropped)*

- **Windows 专用绘制与捕获接口**：`stbid_DrawToHDC` 和 `stbid_CopyFromHDC` – 支持绘制到任意设备上下文，以及从 HDC 捕获图像数据  
  *Windows‑only drawing & capture: `stbid_DrawToHDC` and `stbid_CopyFromHDC` – draw to any HDC and capture image data from HDC*

- **零依赖、极简集成**：仅使用 C 标准库和操作系统原生 API  
  *Zero dependencies, minimal integration: uses only the C standard library and native OS APIs*

---

## 🐞 v1.3.0 主要改进 / Major Improvements

- **新增形态学操作**：膨胀（`stbie_dilate`）与腐蚀（`stbie_erode`），用于图像预处理和后处理。  
  *Added morphological operations: dilation (`stbie_dilate`) and erosion (`stbie_erode`) for image pre‑ and post‑processing.*

- **直方图功能**：直方图均衡化（`stbie_histogram_equalize`）支持灰度与彩色；直方图计算（`stbie_histogram_calc`）支持多通道。  
  *Histogram functions: equalization (`stbie_histogram_equalize`) for grayscale/color; calculation (`stbie_histogram_calc`) for multiple channels.*

- **`stb_image_paint.h` v1.1.1**：新增 `STBIP_NOT_AUTO_INCLUDE_TRUETYPE` 宏，允许用户手动控制 `stb_truetype.h` 的包含，解决多文件项目中的重复定义问题。  
  *`stb_image_paint.h` v1.1.1: added `STBIP_NOT_AUTO_INCLUDE_TRUETYPE` macro to let users manually control inclusion of `stb_truetype.h`, resolving multiple definition issues in multi‑file projects.*

- **`stb_image_display.h` v1.1.1**：修复 Windows 缩放质量（双三次插值），修复 CENTER 模式下窗口大小变化时图像偏移问题。  
  *`stb_image_display.h` v1.1.1: fixed Windows scaling quality (bicubic), fixed image offset in CENTER mode when window resizes.*

---


---

## 📚 API 概览 / API Overview

### `stbie_*`（编辑库 / Editing Library）

| 函数 | 描述（中文） | 描述（英文） |
|------|-------------|-------------|
| `stbie_cvt_color` | 色彩空间转换 | Color space conversion |
| `stbie_filter_2d` | 自定义核卷积 | 2D convolution with custom kernel |
| `stbie_blur_gaussian` | 高斯模糊 | Gaussian blur |
| `stbie_median_blur` | 中值滤波 | Median filter |
| `stbie_blur` | 均值模糊 | Mean blur |
| `stbie_sobel` | Sobel 梯度幅值 | Sobel gradient magnitude |
| `stbie_canny` | Canny 边缘检测（手动阈值） | Canny edge detection (manual thresholds) |
| `stbie_canny_with_adaptive_otsu` | Canny 边缘检测（自动阈值） | Canny edge detection with auto‑threshold |
| `stbie_rotate` | 图像旋转 | Image rotation |
| `stbie_crop` | 图像裁剪 | Image cropping |
| `stbie_dilate` | 膨胀 | Dilation |
| `stbie_erode` | 腐蚀 | Erosion |
| `stbie_histogram_equalize` | 直方图均衡化 | Histogram equalization |
| `stbie_histogram_calc` | 直方图计算 | Histogram calculation |
| `stbie_adaptive_otsu_threshold` | 分块 Otsu 阈值图 | Block‑wise Otsu threshold map |
| `stbie_adaptive_otsu_binarize` | 自适应 Otsu 二值化 | Adaptive Otsu binarization |

---

### `stbip_*`（绘制库 / Painting Library）

| 函数 | 描述（中文） | 描述（英文） |
|------|-------------|-------------|
| `stbip_draw_pixel` | 绘制单个像素 | Draws a single pixel |
| `stbip_draw_line` | Bresenham 线段 | Bresenham line |
| `stbip_draw_rect` | 矩形（填充/边框） | Rectangle (fill/stroke) |
| `stbip_draw_circle` | 圆（填充/轮廓） | Circle (fill/outline) |
| `stbip_draw_polygon` | 多边形（填充/轮廓） | Polygon (fill/stroke) |
| `stbip_draw_text` | ASCII 文本（内置字体） | ASCII text (built‑in font) |
| `stbip_flood_fill` | 泛洪填充 | Flood fill |
| `stbip_clear_image` | 清除图像（别名 `stbip_clear`） | Clear image (alias `stbip_clear`) |
| `stbip_ttf_load` | 加载 TrueType 字体 | Load TrueType font |
| `stbip_ttf_set_current` | 设置当前字体 | Set current font |
| `stbip_draw_text_ttf_utf8` | 绘制 UTF‑8 文本 | Draw UTF‑8 text |
| `stbip_draw_text_ttf_ansi` | 绘制 ANSI 文本 | Draw ANSI text |
| `stbip_draw_text_ttf` | 宏别名（等价于 `ansi`） | Macro alias (equivalent to `ansi`) |
| `stbip_ttf_done` | 释放所有 TrueType 字体 | Free all TrueType fonts |
| `stbip_get_last_error` | 获取错误码 | Get last error code |

---

### `stbid_*`（显示库 / Display Library）

| 函数 | 描述（中文） | 描述（英文） |
|------|-------------|-------------|
| `stbid_Startup` | 初始化窗口子系统 | Initializes windowing subsystem |
| `stbid_Shutdown` | 释放所有资源 | Releases all resources |
| `stbid_ShowImage` | 阻塞式显示 | Blocking display |
| `stbid_ShowImageNonBlocking` | 非阻塞式创建窗口 | Non‑blocking window creation |
| `stbid_UpdateImage` | 更新窗口图像 | Updates window image |
| `stbid_IsWindowOpen` | 检查窗口状态 | Checks window status |
| `stbid_CloseWindow` | 关闭窗口 | Closes window |
| `stbid_PollEvents` | 轮询事件 | Polls events |
| `stbid_DrawToHDC` | （Windows）绘制到 HDC | (Windows) Draw to HDC |
| `stbid_CopyFromHDC` | （Windows）从 HDC 捕获 | (Windows) Capture from HDC |
| `stbid_GetLastError` | 获取错误码 | Gets last error code |

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

3. 如需 TrueType 支持，定义 `STBIP_USE_TRUETYPE`，并根据需要定义 `STBIP_NOT_AUTO_INCLUDE_TRUETYPE` 手动包含 `stb_truetype.h`。  
*For TrueType support, define `STBIP_USE_TRUETYPE`, and optionally define `STBIP_NOT_AUTO_INCLUDE_TRUETYPE` to include `stb_truetype.h` manually.*

4. 编译时链接对应平台的库：  
*Link the appropriate libraries when compiling:*

- **Windows (MinGW/MSVC)**：`-lgdi32 -luser32`
- **Linux**：`-lX11`
- **macOS**：不支持 / Not supported

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

- **v1.4.0**：仿射变换（缩放、平移、剪切）、图像金字塔  
- **v1.5.0**：连通域标记、更多形态学操作（开运算、闭运算）  
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
