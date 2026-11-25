# Native weather greeting experiment

Open repository root in Visual Studio, choose x64-release and Build All.
CLI (x64 Native Tools): cmake --preset x64-release; cmake --build --preset x64-release; ctest --preset x64-release.
The default build copies JinJuApp.exe (XAML launcher), JinJuXaml.dll, JinJuXamlUnload.exe, and JinJuMarquee.exe (original Win32 application) into tests with SHA256 manifests. Close programs and restart Explorer before rebuilding if Windows locks the DLL.
Keep JinJuApp.exe and JinJuXaml.dll together. Run only on a personal Windows 11 test session, with Widgets enabled. Explorer internal templates vary; unknown layouts are left untouched. No Windhawk or external overlay. Diagnostic session success is not proof of insertion. Restart Explorer normally to remove the experimental control and unload the DLL; no registry persistence. The DLL is intentionally resident until Explorer exits. CI tests COM creation and SDK export, not live Explorer visual modifications.

## Cleanup
Run JinJuXamlUnload.exe and confirm the graceful Explorer restart. This is not a remote FreeLibrary operation: the DLL is pinned until the original Explorer process exits. Restart Manager can refuse shell restart; if so use Task Manager > Windows Explorer > Restart. No forced termination. CI tests only Restart Manager session creation; actual shell restart is manual validation.

## P0 diagnostics
After restarting Explorer to unload the previous DLL, run the new JinJuApp.exe from tests. JinJuXaml.log and XamlTree.txt are UTF-8 append-only files beside the DLL/launcher. Delete old logs before a new experiment if desired. Check the logged PID, COM factory, SetSite, Advise, mutation handles and weather snapshots. A nonzero post-dispatch size is not proof of on-screen visibility; a zero size may be transient. No insertion policy or unload changes. Logs contain class/name/layout metadata; review before sharing. CI verifies COM/logging and UTF-8, not live Explorer callbacks.
