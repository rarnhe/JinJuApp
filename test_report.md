# C++ Windows 测试报告

- 状态：通过 (Passed)
- 时间（UTC）：2026-11-24 12:21:23
- 测试源码基准提交：a68646da711e47ee09a1566df78de69953664447
- 执行记录：https://github.com/rarnhe/JinJuApp/actions/runs/45392599052
- 平台：Windows / MSVC / C++20 / Win32 + Windows SDK
- 阶段：Configure=success / Build=success / CTest=success
- 统计：通过 7 / 失败 0 / 跳过 0 / 共 7

## GitHub Actions 阶段状态

| 阶段 | 结果 |
| --- | --- |
| Configure | success |
| Build | success |
| CTest | success |

## 测试用例结果

| 测试 | 结果 |
| --- | --- |
| geometry | 通过 |
| animation | 通过 |
| pixels | 通过 |
| render_pipeline | 通过 |
| window_pixels | 通过 |
| visibility | 通过 |
| quotes | 通过 |

## 失败详情

## 覆盖范围
真实编译 main.cpp，验证动态空闲矩形、动画时间边界、预乘像素、壁纸实际 BGRA 输出、非零屏幕坐标取样、原版 GDI 字形合成、实际 App::Layer 滚动像素变化及 UpdateLayeredWindow 提交、实际双窗口 Z 序恢复、owned popup 堆叠及 WIC 解码、分层窗口样式及 居中 Start 边框重叠 7px、靠左应用组右边界零间距及托盘重叠 7px、动态宽度、窄间距、同名 TXT 创建及 UTF-8 解析。

新增按 Z 序逐像素覆盖、区域裁剪、无窗口白色测试。visibility 测试：双屏全屏状态、最小化/隐藏窗口过滤；不再识别或特殊处理 Win+D / Win+Tab。

实际 Explorer UIA、窗口取色视觉、鼠标穿透、全屏与多屏/DPI 仍需交互式人工验证；不计入自动通过数。
