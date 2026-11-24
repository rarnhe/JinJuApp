# C++ Windows 测试报告

- 状态：失败或未完整执行 (Failed)
- 时间（UTC）：2026-11-24 05:53:32
- 测试源码基准提交：01c674e3d8db82366fb870b3d119dbafae243227
- 执行记录：https://github.com/rarnhe/JinJuApp/actions/runs/45336395134
- 平台：Windows / MSVC / C++20 / Win32 + Windows SDK
- 阶段：Configure=success / Build=failure / CTest=skipped
- 统计：通过 0 / 失败 0 / 跳过 0 / 共 0

## GitHub Actions 阶段状态

| 阶段 | 结果 |
| --- | --- |
| Configure | success |
| Build | failure |
| CTest | skipped |

## 测试用例结果

| 测试 | 结果 |
| --- | --- |
| 未获得 CTest JUnit 结果 | 未执行 |

## 失败详情
没有可解析的 CTest 用例级错误；整体失败原因请查看下面的阶段输出。

### Build 阶段输出
GitHub Actions 错误：Error: Process completed with exit code 1.
<pre>  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(216,8): error C2065: &#39;IStream&#39;: undeclared identifier [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(216,28): error C2065: &#39;stream&#39;: undeclared identifier [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(217,8): error C2275: &#39;HDC&#39;: expected an expression instead of a type [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(217,28): error C2146: syntax error: missing &#39;)&#39; before identifier &#39;referenceHdc&#39; [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(223,1): error C2143: syntax error: missing &#39;;&#39; before &#39;{&#39; [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(223,1): error C2447: &#39;{&#39;: missing function header (old-style formal list?) [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(234,11): error C7550: &#39;Gdiplus::Metafile::{ctor}&#39;: the qualified name in this context names a constructor, not a type [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(235,8): error C2065: &#39;IStream&#39;: undeclared identifier [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(235,28): error C2065: &#39;stream&#39;: undeclared identifier [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(236,8): error C2275: &#39;HDC&#39;: expected an expression instead of a type [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(236,28): error C2146: syntax error: missing &#39;)&#39; before identifier &#39;referenceHdc&#39; [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(242,1): error C2143: syntax error: missing &#39;;&#39; before &#39;{&#39; [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(242,1): error C2447: &#39;{&#39;: missing function header (old-style formal list?) [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(283,11): error C2761: &#39;GetMetafileHeader&#39;: redeclaration of member is not allowed [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(284,8): error C2065: &#39;IStream&#39;: undeclared identifier [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(284,28): error C2065: &#39;stream&#39;: undeclared identifier [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(285,28): error C2065: &#39;header&#39;: undeclared identifier [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(285,9): error C2275: &#39;Gdiplus::MetafileHeader&#39;: expected an expression instead of a type [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(287,1): error C2143: syntax error: missing &#39;;&#39; before &#39;{&#39; [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusMetafile.h(287,1): error C2447: &#39;{&#39;: missing function header (old-style formal list?) [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusBitmap.h(49,8): error C7550: &#39;Gdiplus::Image::{ctor}&#39;: the qualified name in this context names a constructor, not a type [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusBitmap.h(50,8): error C2065: &#39;IStream&#39;: undeclared identifier [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusBitmap.h(50,17): error C2065: &#39;stream&#39;: undeclared identifier [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusBitmap.h(51,8): error C2275: &#39;BOOL&#39;: expected an expression instead of a type [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\GdiplusBitmap.h(51,8): error C1003: error count exceeds 100; stopping compilation [D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\jinju_tests.vcxproj]
  (compiling source file &#39;../../../../tests/test_jinju.cpp&#39;)
  
  main.cpp
  Generating Code...
  JinJuApp.vcxproj -&gt; D:\a\JinJuApp\JinJuApp\out\build\x64-release\Release\JinJuApp.exe
Exit code: 1</pre>

### CTest 阶段输出
阶段状态：skipped（该阶段可能未启动或被取消）。
日志文件不存在：out\test-logs\ctest.log

## 覆盖范围
真实编译 main.cpp，验证动态空闲矩形、动画时间边界、预乘像素、壁纸实际 BGRA 输出、同名 TXT 创建及 UTF-8 解析。

实际 Explorer UIA、壁纸镂空视觉、鼠标穿透、全屏与多屏/DPI 仍需交互式人工验证；不计入自动通过数。
