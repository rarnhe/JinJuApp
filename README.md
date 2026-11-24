# JinJuApp (Win32 C++)
Windows 10/11 x64 原生后台金句覆盖层。无托盘、无注册表自启动、无外部库；任务管理器结束 JinJuApp.exe。不是 Explorer 插件。
- 与 EXE 同目录同名 UTF-8 TXT：缺失时创建默认语料；空文件或读取失败直接退出；每行一句，# 是普通文字；重启加载。不支持 JSON。
- 主显示器底部任务栏：HWND + UI Automation 探测控件矩形，使用最左可用连续区间，无 500px 上限；检测不足或空间不足隐藏。依赖 Explorer 的实际 UIA 树，不能保证覆盖所有 Windows 版本；自动隐藏任务栏不支持。
- 镂空是视觉模拟：通过系统 IDesktopWallpaper 与 GDI+ 读取并定位壁纸，字形显示壁纸对应颜色，字形以外 alpha=0；不能真正穿透 Explorer。壁纸无法读取时隐藏。动态/视频壁纸及幻灯片实时同步不保证。
- 保留 2s 句首、45 逻辑 px/s、0.6s 句尾、250ms 淡出/淡入、400ms 换句交叉淡化、3min 换句。500ms 检测全屏及任务栏；系统通知状态仅使用公开 API 支持的 quiet time/presentation。
- Visual Studio 打开 CMake 项目，选择 x64-release；或在开发者命令行执行：
```powershell
cmake -S . -B out/test -A x64 -DBUILD_TESTING=ON
cmake --build out/test --config Release
ctest --test-dir out/test -C Release --output-on-failure --output-junit test-results.xml
./tests/write_report.ps1
```
CI 每次 PR 执行，始终生成 Markdown 报告及 artifact；main push 写回报告，排除报告循环触发。实际桌面效果需人工验证。
