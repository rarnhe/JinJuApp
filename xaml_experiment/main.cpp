#include "shared.h"
#include "diagnostics.h"
#include "cleanup_request.h"
#include <filesystem>
#include <iostream>
#include <winrt/base.h>
#include <xamlom.h>
int wmain(int argc,wchar_t** argv){
    if(argc==2 && std::wstring_view(argv[1])==L"--remove") return cleanup::removeCurrentGreeting();
    if(argc==2 && std::wstring_view(argv[1])==L"--self-test"){
        auto h=LoadLibraryExW(L"Windows.UI.Xaml.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
        if(!h)return 1;
        auto ok=GetProcAddress(h,"InitializeXamlDiagnosticsEx")!=nullptr;
        FreeLibrary(h);return ok?0:1;
    }
    try{
        winrt::init_apartment();
        DWORD pid=0;
        auto tray=FindWindowW(L"Shell_TrayWnd",nullptr);
        if(!tray || !GetWindowThreadProcessId(tray,&pid))return 2;
        wchar_t exe[32768]{};
        if(!GetModuleFileNameW(nullptr,exe,32768))return 3;
        auto dir=std::filesystem::path(exe).parent_path();
        auto dll=dir/L"JinJuXaml.dll";
        if(!diagnostics::append(dir,L"JinJuXaml.log",L"Launcher: Explorer PID="+std::to_wstring(pid)))std::wcerr<<L"Cannot write diagnostic log\n";
        std::wcout<<L"Log: "<<(dir/L"JinJuXaml.log").wstring()<<L"\nTree: "<<(dir/L"XamlTree.txt").wstring()<<L"\n";
        if(!std::filesystem::exists(dll)){std::wcerr<<L"Missing JinJuXaml.dll\n";return 4;}
        auto module=LoadLibraryExW(L"Windows.UI.Xaml.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
        if(!module)return 5;
        using Init=HRESULT(WINAPI*)(LPCWSTR,DWORD,LPCWSTR,LPCWSTR,CLSID,LPCWSTR);
        auto init=reinterpret_cast<Init>(GetProcAddress(module,"InitializeXamlDiagnosticsEx"));
        if(!init)return 6;
        wchar_t system[32768]{};
        if(!GetSystemDirectoryW(system,32768))return 7;
        auto xaml=std::filesystem::path(system)/L"Windows.UI.Xaml.dll";
        HRESULT hr=E_FAIL;
        unsigned connected=0;
        for(int i=1;i<=20;++i){
            auto endpoint=L"VisualDiagConnection"+std::to_wstring(i);
            hr=init(endpoint.c_str(),pid,xaml.c_str(),dll.c_str(),TapId,L"");
            diagnostics::append(dir,L"JinJuXaml.log",endpoint+L" HRESULT="+diagnostics::hr(hr));
            if(SUCCEEDED(hr))++connected;
            // Other names may refer to separate XAML diagnostic endpoints.
            // Keep the search bounded; log every result, not just the first success.
        }
        std::wcout<<L"Last endpoint HRESULT: 0x"<<std::hex<<static_cast<unsigned>(hr)<<L"\n";
        std::wcout<<L"Successful endpoints: "<<std::dec<<connected<<L"/20 (not proof of target discovery)\n";
        return connected?0:8;
    }catch(winrt::hresult_error const& e){diagnostics::append(diagnostics::directory(),L"JinJuXaml.log",L"Launcher exception HRESULT="+diagnostics::hr(e.code())+L" "+e.message().c_str());return 9;}catch(...){diagnostics::append(diagnostics::directory(),L"JinJuXaml.log",L"Launcher unknown exception");return 9;}
}
