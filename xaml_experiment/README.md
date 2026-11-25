# Native weather greeting experiment
Build: cmake -S xaml_experiment -B out/build/xaml-release -A x64
Then: cmake --build out/build/xaml-release --config Release
Keep JinJuApp.exe and JinJuXaml.dll together. Run only on a personal Windows 11 test session, with Widgets enabled. Explorer internal templates vary; unknown layouts are left untouched. No Windhawk or external overlay. Diagnostic session success is not proof of insertion. Restart Explorer normally to remove the experimental control and unload the DLL; no registry persistence. The DLL is intentionally resident until Explorer exits. CI tests COM creation and SDK export, not live Explorer visual modifications.

## Cleanup
Run JinJuXamlUnload.exe and confirm the graceful Explorer restart. This is not a remote FreeLibrary operation: the DLL is pinned until the original Explorer process exits. Restart Manager can refuse shell restart; if so use Task Manager > Windows Explorer > Restart. No forced termination. CI tests only Restart Manager session creation; actual shell restart is manual validation.
