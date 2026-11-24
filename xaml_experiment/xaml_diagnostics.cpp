#include "shared.h"
#include <ocidl.h>
#include <xamlom.h>
#undef GetCurrentTime
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Foundation.Collections.h>
using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
namespace {
void AddGreeting(FrameworkElement const& button){
    // Only modify the named weather content grid; append a column without replacing children.
    auto scan=[&](auto&& self,DependencyObject const& node)->Grid{
        if(auto g=node.try_as<Grid>();g && g.Name()==L"AugmentedEntryPointContentGrid")return g;
        for(int i=0;i<Media::VisualTreeHelper::GetChildrenCount(node);++i)
            if(auto g=self(self,Media::VisualTreeHelper::GetChild(node,i)))return g;
        return nullptr;
    };
    auto grid=scan(scan,button);
    if(!grid)return;
    auto children=grid.Children();
    for(auto const& child:children)
        if(auto e=child.try_as<FrameworkElement>();e && e.Name()==Marker)return;
    auto cols=grid.ColumnDefinitions();
    if(!cols.Size())return; // Unknown template: leave it untouched.
    TextBlock text;
    text.Name(Marker);text.Text(Greeting);
    text.Margin(Thickness{6,0,0,0});
    text.VerticalAlignment(VerticalAlignment::Center);
    text.IsHitTestVisible(false);
    ColumnDefinition column;
    column.Width(GridLength{1,GridUnitType::Auto});
    auto index=cols.Size();
    Grid::SetColumn(text,index);
    Grid::SetRowSpan(text,static_cast<int>(grid.RowDefinitions().Size()?grid.RowDefinitions().Size():1));
    cols.Append(column);
    try{children.Append(text);}catch(...){cols.RemoveAt(index);throw;}
    OutputDebugStringW(L"JinJuXaml: greeting appended\n");
}
struct Watcher:implements<Watcher,IVisualTreeServiceCallback,non_agile>{
    com_ptr<IXamlDiagnostics> diag;
    explicit Watcher(com_ptr<IXamlDiagnostics> d):diag(std::move(d)){}
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation,VisualElement e,VisualMutationType mutation) noexcept override{
        if(mutation!=Add)return S_OK;
        try{
            com_ptr<::IInspectable> raw;
            if(FAILED(diag->GetIInspectableFromHandle(e.Handle,raw.put())))return S_OK;
            Windows::Foundation::IInspectable obj{nullptr};
            copy_from_abi(obj,raw.get());
            auto element=obj.try_as<FrameworkElement>();
            if(!element)return S_OK;
            // A descendant notification can arrive after the weather template is constructed.
            for(auto current=element;current;){
                if(WeatherButton(get_class_name(current))){
                    auto dispatcher=current.Dispatcher();
                    auto weak=make_weak(current);
                    dispatcher.RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,[weak](){
                        try{if(auto b=weak.get())AddGreeting(b);}catch(...){}
                    });
                    break;
                }
                auto p=Media::VisualTreeHelper::GetParent(current);
                current=p?p.try_as<FrameworkElement>():nullptr;
            }
        }catch(...){}
        return S_OK;
    }
};
struct Tap:implements<Tap,IObjectWithSite,non_agile>{
    com_ptr<IUnknown> site;
    com_ptr<IVisualTreeService> service;
    com_ptr<Watcher> watcher;
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* value) noexcept override{
        try{
            if(service && watcher)service->UnadviseVisualTreeChange(watcher.get());
            watcher=nullptr;service=nullptr;site=nullptr;
            if(!value)return S_OK;
            site.copy_from(value);
            auto d=site.as<IXamlDiagnostics>();
            service=site.as<IVisualTreeService>();
            watcher=make_self<Watcher>(d);
            // Keep module resident while queued UI callbacks exist.
            HMODULE pinned{};
            GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
                reinterpret_cast<LPCWSTR>(&AddGreeting),&pinned);
            return service->AdviseVisualTreeChange(watcher.get());
        }catch(...){return to_hresult();}
    }
    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid,void** out) noexcept override{
        if(!out)return E_POINTER;*out=nullptr;
        return site?site->QueryInterface(iid,out):E_FAIL;
    }
};
struct Factory:implements<Factory,IClassFactory>{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer,REFIID iid,void** out) noexcept override{
        if(!out)return E_POINTER;*out=nullptr;
        if(outer)return CLASS_E_NOAGGREGATION;
        try{return make_self<Tap>()->QueryInterface(iid,out);}catch(...){return to_hresult();}
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override{return S_OK;}
};
}
STDAPI DllGetClassObject(REFCLSID cls,REFIID iid,void** out){
    if(!out)return E_POINTER;*out=nullptr;
    if(!IsEqualCLSID(cls,TapId))return CLASS_E_CLASSNOTAVAILABLE;
    try{return make_self<Factory>()->QueryInterface(iid,out);}catch(...){return to_hresult();}
}
STDAPI DllCanUnloadNow(){return S_FALSE;}
