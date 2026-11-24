# C++ Windows 测试报告

- 状态：通过 (Passed)
- 时间（UTC）：2026-11-24 08:30:07
- 测试源码基准提交：7b43714f685eb98f361f8640243c4748a940f664
- 执行记录：https://github.com/rarnhe/JinJuApp/actions/runs/45356561117
- 平台：Windows / MSVC / C++20 / Win32 + Windows SDK
- 阶段：Configure=success / Build=success / CTest=success
- 统计：通过 5 / 失败 0 / 跳过 0 / 共 5

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
| quotes | 通过 |

## 失败详情

## 覆盖范围
真实编译 main.cpp，验证动态空闲矩形、动画时间边界、预乘像素、壁纸实际 BGRA 输出、非零屏幕坐标取样、原版 GDI 字形合成、实际 App::Layer 滚动像素变化及 UpdateLayeredWindow 提交、实际双窗口 Z 序恢复、owned popup 堆叠及 WIC 解码、分层窗口样式及 居中 Start 边框重叠 7px、靠左应用组右边界零间距及托盘重叠 7px、动态宽度、窄间距、同名 TXT 创建及 UTF-8 解析。

实际 Explorer UIA、壁纸镂空视觉、鼠标穿透、全屏与多屏/DPI 仍需交互式人工验证；不计入自动通过数。
