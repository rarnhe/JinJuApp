# Native weather greeting experiment

Open repository root in Visual Studio, choose x64-release and Build All.
CLI (x64 Native Tools): cmake --preset x64-release; cmake --build --preset x64-release; ctest --preset x64-release.
The default build copies JinJuApp.exe (XAML launcher), JinJuXaml.dll, JinJuXamlUnload.exe, and JinJuMarquee.exe (original Win32 application) into tests with SHA256 manifests. Close programs and restart Explorer before rebuilding if Windows locks the DLL.
Keep JinJuApp.exe and JinJuXaml.dll together. Run JinJuApp.exe in a Windows 11 session with Widgets enabled. This experiment displays the native greeting 你好。 beside the weather card.

## Weather layout

The UI thread samples the named root panel, content grid and measured Adaptive Cards at 250 ms intervals. Four matching samples allow one TextBlock to be appended beside the content grid. Only the greeting receives a render transform. It stays transparent while measuring or when the button has insufficient room to its right. Existing weather children, columns and hit testing retain their original values.

Template replacement and button reloads restart the same stability check. A live layout update moves the existing greeting as space changes. JinJuXaml.log records state changes and placement. XamlTree.txt contains a bounded weather snapshot at a stable insertion attempt. File output runs in batches.

## Remove the greeting

From the repository root, run `.\tests\JinJuApp.exe --remove` or double-click `tests/JinJuXamlUnload.exe`. The command asks the active JinJu receiver in the current taskbar session to stop observing, cancel UI work and remove its own TextBlock. It returns success after the active dispatchers acknowledge cleanup. An absent receiver is reported separately. A timeout or incomplete cleanup returns a failure code and can be retried. Launch JinJuApp.exe again to enable the greeting.

The loaded experiment DLL remains pinned until Explorer exits. To replace a locked DLL or clear a version predating the cleanup command, `.\tests\JinJuXamlUnload.exe --restart-explorer` offers the original confirmed Explorer restart.

## Visual checks on Windows 11

1. Open Widgets from the weather button, hover over it, and watch it update with the greeting displayed.
2. Resize the available taskbar area. The greeting should only appear when the full text fits to the right of the weather.
3. Disable and re-enable Widgets. After the card settles, check for a single greeting.
4. Run `--remove` twice. Check the weather remains usable, then launch again and verify a single greeting returns.

CI exercises the native XAML objects in its own process and checks the release file hashes. The live shell appearance, custom panel arrangement and weather flyout need an interactive Windows 11 session.

## Read-only security diagnostics
The earlier visual baseline was f34f302b4a3f30731fd9db7b4d738ff135bc8f87. Keep Defender real-time protection enabled. From PowerShell run `./tests/JinJuSecurityReport.ps1 -Hours 2` after an attempted normal launch. If access to Defender queries is denied, run the report in an administrator PowerShell; the script never elevates itself. It writes tests/JinJuSecurity.log (overwritten each run), including Defender status, binary hashes/signatures, product-related detections/events, and recent XAML milestones. Review paths and event messages before sharing. No protection changes, exclusions, remediation, execution of binaries, or network uploads. CI self-test uses synthetic data and does not query Defender. An empty result is not proof of no detection. Submit suspected false positives to Microsoft for review.

## Security capture follow-up
The report at ab182cf contains no matching Defender detections and all four hashes match. The latest launch at 18:01 UTC successfully initialized the DLL and appended a TextBlock about 24 seconds later. Defender status was sampled at 18:07 UTC, not during launch; causation is unproven. The revised report distinguishes empty event queries from failures, includes historical real-time protection state changes and product-related Code Integrity/AppLocker records, and filters XAML timestamps to the requested window. Use `-Since` with a local timestamp immediately before a normal launch with protection enabled. Do not disable protections. Submit confirmed suspected false positives to Microsoft.
