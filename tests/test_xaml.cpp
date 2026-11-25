#include "../xaml_experiment/shared.h"
#include "../xaml_experiment/diagnostics.h"
#include <fstream>
#include <iterator>
#include <ocidl.h>
#include <iostream>
int wmain(int argc,wchar_t** argv){
    if(argc!=2 || !WeatherButton(L"Taskbar.AugmentedEntryPointButton") ||
       WeatherButton(L"Taskbar.TaskListButton") || std::wstring_view(Greeting)!=L"你好。")return 1;
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
    auto dir=std::filesystem::path(argv[1]).parent_path();
    std::ifstream log(dir/L"JinJuXaml.log",std::ios::binary);
    std::string contents((std::istreambuf_iterator<char>(log)),{});
    if(contents.find("DllGetClassObject HRESULT=0x00000000")==std::string::npos || contents.find("SetSite HRESULT=0x00000000")==std::string::npos)return 7;
    if(diagnostics::hr(E_FAIL)!=L"0x80004005")return 8;
    if(!diagnostics::append(dir,L"diagnostics-test.txt",L"你好。"))return 9;
    std::ifstream utf(dir/L"diagnostics-test.txt",std::ios::binary);
    std::string bytes((std::istreambuf_iterator<char>(utf)),{});
    return bytes.find("\xe4\xbd\xa0\xe5\xa5\xbd\xe3\x80\x82")==std::string::npos?10:0;
}
