# C++ Windows 测试报告

- 状态：失败（报告生成脚本异常，已写入降级报告）
- Configure=success / Build=success / CTest=success

## 报告生成错误
<pre>ParserError: D:\a\JinJuApp\JinJuApp\tests\write_report.ps1:134
Line |
 134 |  $lines.Add(&quot;实际 Explorer UIA、壁纸镂空视觉、全屏与多屏/DPI 仍需交互式人工验证；不计入自动通过数。
     |              ~
     | Missing &#39;)&#39; in method call.

</pre>

### configure 最后 80 行
<pre>-- Building for: Visual Studio 18 2026
-- The CXX compiler identification is MSVC 19.52.37029.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/18/Enterprise/VC/Tools/MSVC/14.52.37025/bin/Hostx64/x64/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (15.8s)
-- Generating done (0.0s)
-- Build files have been written to: D:/a/JinJuApp/JinJuApp/out/build/x64-release
Exit code: 0</pre>

### build 最后 80 行
<pre>MSBuild version 18.12.2-1.26514.2+04f6ca984 for .NET Framework

  1&gt;Checking Build System
  Building Custom Rule D:/a/JinJuApp/JinJuApp/CMakeLists.txt
  main.cpp
  Building Custom Rule D:/a/JinJuApp/JinJuApp/tests/CMakeLists.txt
  test_jinju.cpp
  main.cpp
  Generating Code...
  JinJuApp.vcxproj -&gt; D:\a\JinJuApp\JinJuApp\out\build\x64-release\Release\JinJuApp.exe
  jinju_tests.vcxproj -&gt; D:\a\JinJuApp\JinJuApp\out\build\x64-release\tests\Release\jinju_tests.exe
  Building Custom Rule D:/a/JinJuApp/JinJuApp/CMakeLists.txt
Exit code: 0</pre>

### ctest 最后 80 行
<pre>Test project D:/a/JinJuApp/JinJuApp/out/build/x64-release
    Start 1: geometry
1/4 Test #1: geometry .........................   Passed    0.02 sec
    Start 2: animation
2/4 Test #2: animation ........................   Passed    0.01 sec
    Start 3: pixels
3/4 Test #3: pixels ...........................   Passed    0.01 sec
    Start 4: quotes
4/4 Test #4: quotes ...........................   Passed    0.01 sec

100% tests passed out of 4

Total Test time (real) =   0.11 sec
Exit code: 0</pre>
