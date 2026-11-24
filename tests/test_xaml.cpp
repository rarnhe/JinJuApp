#include "../xaml_experiment/shared.h"
#include <ocidl.h>
#include <iostream>
int wmain(int argc,wchar_t** argv){
    if(argc!=2 || !WeatherButton(L"Taskbar.AugmentedEntryPointButton") ||
       WeatherButton(L"Taskbar.TaskListButton") || std::wstring_view(Greeting)!=L"你好。")return 1;
    auto dll=LoadLibraryW(argv[1]);if(!dll)return 2;
    using Get=HRESULT(WINAPI*)(REFCLSID,REFIID,void**);
    auto get=reinterpret_cast<Get>(GetProcAddress(dll,"DllGetClassObject"));
    if(!get)return 3;
    IClassFactory* factory=nullptr;
    if(FAILED(get(TapId,IID_IClassFactory,reinterpret_cast<void**>(&factory))))return 4;
    IObjectWithSite* tap=nullptr;
    auto hr=factory->CreateInstance(nullptr,IID_IObjectWithSite,reinterpret_cast<void**>(&tap));
    if(FAILED(hr))return 5;
    auto clear=tap->SetSite(nullptr);
    tap->Release();factory->Release();FreeLibrary(dll);
    return FAILED(clear)?6:0;
}
