# Linjian-stb-extension

**stb 风格的单头文件图像处理与绘制工具集**  
**A collection of stb‑style single‑header libraries for image processing and painting**

---

## 📖 概述 / Overview

**Linjian-stb-extension** 是一组单头文件的 C 语言库，专为图像处理与绘制设计。它延续了 stb 家族“单文件、零依赖”的哲学，为游戏开发、嵌入式系统或任何需要轻量级图像操作的项目提供即插即用的解决方案。

目前包含 **`stb_image_edit.h`**（色彩转换、滤波、边缘检测、几何变换）和计划中的 **`stb_image_paint.h`**（基本绘图原语）。未来还将扩展更多功能，如图像金字塔、形态学操作等。

**Linjian-stb-extension** is a set of single‑header C libraries for image processing and drawing. It follows the stb philosophy: “single file, zero dependencies” and provides plug‑and‑play solutions for game development, embedded systems, or any project that needs lightweight image manipulation.

It currently includes **`stb_image_edit.h`** (color conversion, filtering, edge detection, geometric transforms) and the planned **`stb_image_paint.h`** (basic drawing primitives). More features (image pyramids, morphological operations, etc.) will be added in the future.

---

## ✨ 特性 / Features

### 已实现 / Implemented (`stb_image_edit.h`)
- **色彩转换**：RGB ↔ 灰度 / YUV / HSV / BGR  
- **滤波**：高斯模糊、中值滤波、均值模糊、自定义卷积  
- **边缘检测**：Sobel 及完整 Canny 算法  
- **几何变换**：图像旋转（双线性插值，裁剪/扩展模式）  
- **裁剪**与**安全像素访问**  
- **错误处理**与**性能优化**（定点整数、可分离卷积）

### 计划中 / Planned
- **`stb_image_paint.h`**：绘制线、矩形、圆、多边形、文本等  
- **`stb_image_extra.h`**：直方图均衡、形态学操作、图像金字塔  
- **`stb_image_transform.h`**：仿射变换（缩放、平移、剪切）

---

## 🚀 快速开始 / Quick Start

1. **下载所需头文件**（例如 `stb_image_edit.h`）。  
2. 在 **一个** `.c` 文件中定义实现宏并包含头文件：  
   #define STB_IMAGE_EDIT_IMPLEMENT
   #include "stb_image_edit.h"
3. 编译时链接数学库 -lm。

如需加载/保存图像，请同时使用 stb_image.h 和 stb_image_write.h（来自 stb 仓库）。

## 📚 API 概览（以 stb_image_edit 为例）/ API Overview (for stb_image_edit)
### **色彩转换** / Color Conversion
int stbie_cvt_color(const stbi_uc *input, int w, int h, int channels_in,
                    stbi_uc *output, int channels_out, int code);

                    
### **滤波** / Filtering
int stbie_filter_2d(const stbi_uc *input, int w, int h, int channels,
                    stbi_uc *output,
                    const float *kernel, int k_w, int k_h,
                    int border_mode);
int stbie_blur_gaussian(...);
int stbie_median_blur(...);
int stbie_blur(...);


### **边缘检测** / Edge Detection
int stbie_sobel(const stbi_uc *input, int w, int h, int channels,
                stbi_uc *output, int border_mode);
int stbie_canny(...);


### **旋转** / Rotation
void stbie_rotate_calc_size(...);
int stbie_rotate(...);

### **裁剪与像素访问** / Crop & Pixel Access
int stbie_crop(...);
int stbie_get_pixel(...);
int stbie_set_pixel(...);
完整文档请参见各头文件顶部的注释。

## 🛠️ 集成方法 / Integration
将所需的 .h 文件复制到您的项目目录。

在 且仅在 一个 .c 文件中定义实现宏（如 STB_IMAGE_EDIT_IMPLEMENT），然后 #include 对应的头文件。

如果使用多个子库，每个子库都需要在各自的 .c 文件中定义其实现宏（或集中定义，但需确保不重复包含）。

编译时链接 -lm。

## 📄 许可证 / License
所有库均采用 MIT 许可证，允许自由使用（含商业项目）。详见 LICENSE 文件。

## 🙏 致谢 / Credits
基于 Sean Barrett 的 stb 项目（公共领域）而构建。

灵感源自 OpenCV。

由 Linjian Studio（@coder-linjian）开发维护。

## 🗺️ 路线图 / Roadmap
v1.1.0：增加 stb_image_paint.h（绘图原语）

v1.2.0：增加直方图均衡、形态学操作

v1.3.0：仿射变换、图像金字塔

未来：可选 SIMD 加速（通过编译宏）

## 🤝 贡献 / Contributing
欢迎提交 Issue 和 Pull Request。请确保代码风格一致并附上适当注释。

## 📬 联系方式 / Contact
如有疑问或建议，请在 GitHub 上提交 Issue。

## 祝您图像处理愉快！
Happy image processing! 🖼️
