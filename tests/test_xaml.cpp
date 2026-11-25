#include "../xaml_experiment/shared.h"
#include "../xaml_experiment/async_log.h"
#include "../xaml_experiment/layout_policy.h"
#include <limits>
#include <fstream>
#include <iterator>
#include <ocidl.h>
#include <iostream>
int wmain(int argc,wchar_t** argv){
    if (argc==2 && std::wstring_view(argv[1])==L"--log-queue") {
        diagnostics::LogQueue queue;
        for (size_t i=0; i<queue.capacity+3; ++i)
            queue.push({diagnostics::LogFile::tree, {}, 42, std::to_wstring(i)});
        size_t next=0;
        while (!queue.empty()) {
            uint64_t lost=0;
            auto batch=queue.take(lost);
            if (batch.size()!=queue.batchSize || lost!=(next==0 ? 3u : 0u)) return 12;
            for (auto const& line : batch)
                if (line.thread!=42 || line.text!=std::to_wstring(next++)) return 13;
        }
        return next==queue.capacity ? 0 : 14;
    }
    if(argc!=2 || !WeatherButton(L"Taskbar.AugmentedEntryPointButton") ||
       WeatherButton(L"Taskbar.TaskListButton") || std::wstring_view(Greeting)!=L"你好。")return 1;
    if(!layout::validSize(30,20)||layout::validSize(0,20)||
       layout::validSize(std::numeric_limits<double>::infinity(),20)||
       !layout::inside(60,0,30,20,100,40)||layout::inside(90,0,30,20,100,40)||
       !layout::weatherGrid(L"AugmentedEntryPointContentGrid")||layout::weatherGrid(L"OverflowRootGrid"))return 11;
    auto dir=std::filesystem::path(argv[1]).parent_path();
    const auto logPath=dir/L"JinJuXaml.log";
    const auto start=std::filesystem::exists(logPath) ? std::filesystem::file_size(logPath) : 0;
    auto dll=LoadLibraryW(argv[1]);if(!dll)return 2;
    using Get=HRESULT(WINAPI*)(REFCLSID,REFIID,void**);
    auto get=reinterpret_cast<Get>(GetProcAddress(dll,"DllGetClassObject"));
    if(!get || !GetProcAddress(dll,"DllCanUnloadNow"))return 3;
    IClassFactory* factory=nullptr;
    if(FAILED(get(TapId,IID_IClassFactory,reinterpret_cast<void**>(&factory))))return 4;
    IObjectWithSite* tap=nullptr;
    auto hr=factory->CreateInstance(nullptr,IID_IObjectWithSite,reinterpret_cast<void**>(&tap));
    if(FAILED(hr))return 5;
    auto clear=tap->SetSite(nullptr);
    tap->Release();factory->Release();FreeLibrary(dll);
    if(FAILED(clear))return 6;
    bool written=false;
    for (int tries=0; tries<100 && !written; ++tries) {
        std::ifstream log(logPath,std::ios::binary);
        if (log) {
            log.seekg(static_cast<std::streamoff>(start));
            std::string contents((std::istreambuf_iterator<char>(log)),{});
            written=contents.find("DllGetClassObject HRESULT=0x00000000")!=std::string::npos &&
                    contents.find("SetSite HRESULT=0x00000000")!=std::string::npos;
        }
        if (!written) Sleep(50);
    }
    if (!written) return 7;
    if(diagnostics::hr(E_FAIL)!=L"0x80004005")return 8;
    if(!diagnostics::append(dir,L"diagnostics-test.txt",L"你好。"))return 9;
    std::ifstream utf(dir/L"diagnostics-test.txt",std::ios::binary);
    std::string bytes((std::istreambuf_iterator<char>(utf)),{});
    return bytes.find("\xe4\xbd\xa0\xe5\xa5\xbd\xe3\x80\x82")==std::string::npos?10:0;
}
