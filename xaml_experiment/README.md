# Native weather greeting experiment
Build: cmake -S xaml_experiment -B out/build/xaml-release -A x64
Then: cmake --build out/build/xaml-release --config Release
Keep JinJuApp.exe and JinJuXaml.dll together. Run only on a personal Windows 11 test session, with Widgets enabled. Explorer internal templates vary; unknown layouts are left untouched. No Windhawk or external overlay. Diagnostic session success is not proof of insertion. Restart Explorer normally to remove the experimental control and unload the DLL; no registry persistence. The DLL is intentionally resident until Explorer exits. CI tests COM creation and SDK export, not live Explorer visual modifications.
