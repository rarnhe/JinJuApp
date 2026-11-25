#include "shared.h"
#include "async_log.h"
#include <algorithm>
#include "layout_policy.h"
#include <chrono>
#include <vector>
#include <cstring>
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
diagnostics::AsyncLog& Logger() {
    static diagnostics::AsyncLog log;
    return log;
}
void Log(std::wstring_view message) noexcept {
    Logger().push(diagnostics::LogFile::events, message);
}
void LogHr(std::wstring_view stage,HRESULT value) noexcept {
    try{Log(std::wstring(stage)+L" HRESULT="+diagnostics::hr(value));}catch(...){}
}
void LogException(std::wstring_view stage) noexcept {
    try {
        throw;
    } catch(hresult_error const& e){
        try{Log(std::wstring(stage)+L" HRESULT="+diagnostics::hr(e.code())+
                L" message="+diagnostics::clean(e.message().c_str()));}catch(...){}
    } catch(std::exception const& e){
        try{Log(std::wstring(stage)+L" exception="+std::wstring(e.what(),e.what()+strlen(e.what())));}catch(...){}
    } catch(...){Log(stage);}
}
// Read-only snapshot of actual XAML elements. Parent and child handles in the
// callback log reconstruct the full observed tree, while the local snapshot
// captures layout of the weather subtree and its ancestors.
void Describe(FrameworkElement const& e,std::wostringstream& s){
    s<<L"class="<<diagnostics::clean(get_class_name(e).c_str())
     <<L" name="<<diagnostics::clean(e.Name().c_str())
     <<L" actual="<<e.ActualWidth()<<L"x"<<e.ActualHeight()
     <<L" width="<<e.Width()<<L" height="<<e.Height()
     <<L" minWidth="<<e.MinWidth()<<L" maxWidth="<<e.MaxWidth()
     <<L" minHeight="<<e.MinHeight()<<L" maxHeight="<<e.MaxHeight()
     <<L" visibility="<<static_cast<int>(e.Visibility())<<L" opacity="<<e.Opacity()
     <<L" clip="<<static_cast<bool>(e.Clip())
     <<L" column="<<Grid::GetColumn(e)<<L" columnSpan="<<Grid::GetColumnSpan(e)
     <<L" row="<<Grid::GetRow(e)<<L" rowSpan="<<Grid::GetRowSpan(e)
     <<L" visualChildren="<<Media::VisualTreeHelper::GetChildrenCount(e);
    if(auto grid=e.try_as<Grid>()){
        s<<L" columns="<<grid.ColumnDefinitions().Size()<<L" rows="<<grid.RowDefinitions().Size()
         <<L" children="<<grid.Children().Size();
        for(auto const& col:grid.ColumnDefinitions()){
            auto width=col.Width();
            s<<L" [column width="<<width.Value<<L" unit="<<static_cast<int>(width.GridUnitType)
             <<L" actual="<<col.ActualWidth()<<L"]";
        }
        for(auto const& row:grid.RowDefinitions()){
            auto height=row.Height();
            s<<L" [row height="<<height.Value<<L" unit="<<static_cast<int>(height.GridUnitType)
             <<L" actual="<<row.ActualHeight()<<L"]";
        }
    }
}
void TreeLine(std::wstring_view line) noexcept {
    Logger().push(diagnostics::LogFile::tree, line);
}
void DumpElement(FrameworkElement const& e,unsigned depth,unsigned& budget){
    if(!budget||depth>64)return;
    --budget;
    std::wostringstream s;s<<std::wstring(depth*2,L' ');
    Describe(e,s);TreeLine(s.str());
    for(int i=0;i<Media::VisualTreeHelper::GetChildrenCount(e)&&budget;++i){
        if(auto child=Media::VisualTreeHelper::GetChild(e,i).try_as<FrameworkElement>())
            DumpElement(child,depth+1,budget);
    }
}
void DumpWeather(FrameworkElement const& button,std::wstring_view stage) noexcept {
    try{
        TreeLine(std::wstring(L"=== weather snapshot ")+std::wstring(stage)+L" ===");
        unsigned budget=512;
        DumpElement(button,0,budget);
        if(!budget)TreeLine(L"snapshot truncated at 512 elements");
        auto parent=Media::VisualTreeHelper::GetParent(button);
        for(unsigned depth=0;parent&&depth<64;++depth){
            if(auto e=parent.try_as<FrameworkElement>()){
                std::wostringstream s;s<<L"ancestor "<<depth<<L" ";Describe(e,s);TreeLine(s.str());
            }
            parent=Media::VisualTreeHelper::GetParent(parent);
        }
    }catch(...){LogException(L"DumpWeather");}
}
void ObserveGreeting(TextBlock const& text,Grid const& grid,FrameworkElement const& button){
    auto weakText=make_weak(text);
    auto weakGrid=make_weak(grid);
    auto weakButton=make_weak(button);
    DispatcherTimer timer;
    timer.Interval(std::chrono::milliseconds(250));
    timer.Tick([weakText,weakGrid,weakButton,attempt=0](Windows::Foundation::IInspectable const& sender,Windows::Foundation::IInspectable const&) mutable {
        auto clock=sender.as<DispatcherTimer>();
        ++attempt;
        if(attempt>=8)clock.Stop();
        try{
            auto t=weakText.get();auto g=weakGrid.get();auto b=weakButton.get();
            if(!t||!g||!b){clock.Stop();Log(L"P1 layout: target removed");return;}
            auto w=t.ActualWidth(),h=t.ActualHeight();
            bool size=layout::validSize(w,h),visible=true,clip=false;
            bool attached=false;
            for(DependencyObject node=t;node;node=Media::VisualTreeHelper::GetParent(node)){
                if(node==b)attached=true;
                if(auto ui=node.try_as<UIElement>()){
                    visible=visible&&ui.Visibility()==Visibility::Visible&&ui.Opacity()>0;
                    clip=clip||static_cast<bool>(ui.Clip());
                }
            }
            bool inGrid=false,inButton=false;
            if(size&&attached){
                Windows::Foundation::Rect rect{0,0,static_cast<float>(w),static_cast<float>(h)};
                auto rg=t.TransformToVisual(g).TransformBounds(rect);
                auto rb=t.TransformToVisual(b).TransformBounds(rect);
                inGrid=layout::inside(rg.X,rg.Y,rg.Width,rg.Height,g.ActualWidth(),g.ActualHeight());
                inButton=layout::inside(rb.X,rb.Y,rb.Width,rb.Height,b.ActualWidth(),b.ActualHeight());
            }
            bool candidate=size&&visible&&attached&&inGrid&&inButton&&!clip;
            std::wostringstream s;s<<L"P1 layout attempt="<<attempt<<L" ";Describe(t,s);
            s<<L" grid="<<g.ActualWidth()<<L"x"<<g.ActualHeight()
             <<L" button="<<b.ActualWidth()<<L"x"<<b.ActualHeight()
             <<L" validSize="<<size<<L" ancestorVisible="<<visible<<L" attached="<<attached
             <<L" insideGrid="<<inGrid<<L" insideButton="<<inButton<<L" ancestorClip="<<clip
             <<L" visibilityCandidate="<<candidate<<L" visualProof=manual";
            Log(s.str());TreeLine(s.str());
        }catch(...){LogException(L"P1 layout observation");}
    });
    timer.Start();
}
void AddGreeting(FrameworkElement const& button){
    // Only modify the named weather content grid; append a column without replacing children.
    auto scan=[&](auto&& self,DependencyObject const& node)->Grid{
        if(auto g=node.try_as<Grid>();g && layout::weatherGrid(g.Name()))return g;
        for(int i=0;i<Media::VisualTreeHelper::GetChildrenCount(node);++i)
            if(auto g=self(self,Media::VisualTreeHelper::GetChild(node,i)))return g;
        return nullptr;
    };
    auto grid=scan(scan,button);
    if(!grid){Log(L"WeatherGrid not found");return;}
    auto children=grid.Children();
    for(auto const& child:children)
        if(auto e=child.try_as<FrameworkElement>();e && e.Name()==Marker){Log(L"TextBlock already present");return;}
    auto cols=grid.ColumnDefinitions();
    const auto originalCount=cols.Size();
    for(auto const& child:children){
        // UIElementCollection returns UIElement; the SDK Grid getters require FrameworkElement.
        auto element=child.try_as<FrameworkElement>();
        if(!element){Log(L"P1 skip: existing Grid child is not a FrameworkElement");return;}
        if(!layout::preservesColumn(originalCount,Grid::GetColumn(element),Grid::GetColumnSpan(element))){
            Log(L"P1 skip: existing child column/span would change meaning");return;
        }
    }
    if(!children.Size()){Log(L"P1 skip: weather content not constructed");return;}
    Log(L"P1 weather Grid original columns="+std::to_wstring(originalCount)+L" children="+std::to_wstring(children.Size()));
    TextBlock text;
    text.Name(Marker);text.Text(Greeting);
    text.Margin(Thickness{6,0,0,0});
    text.VerticalAlignment(VerticalAlignment::Center);
    text.IsHitTestVisible(false);
    ColumnDefinition column;
    column.Width(GridLength{1,GridUnitType::Auto});
    auto index=originalCount?originalCount:1u;
    Grid::SetColumn(text,index);
    Grid::SetRowSpan(text,static_cast<int>(grid.RowDefinitions().Size()?grid.RowDefinitions().Size():1));
    try{
        if(!originalCount){
            ColumnDefinition implicit;
            implicit.Width(GridLength{1,GridUnitType::Star});
            cols.Append(implicit);
        }
        cols.Append(column);
        children.Append(text);
    }catch(...){
        while(cols.Size()>originalCount)cols.RemoveAtEnd();
        throw;
    }
    OutputDebugStringW(L"JinJuXaml: greeting appended\n");
    Log(L"TextBlock appended; actual size pending layout");
    try{ObserveGreeting(text,grid,button);}catch(...){LogException(L"Queue P1 layout observation");}
}
void QueueWeather(FrameworkElement const& element){
    // Coalesce the button and its children's notifications on this UI dispatcher.
    static thread_local std::vector<weak_ref<FrameworkElement>> pending;
    std::erase_if(pending, [](auto const& item) { return !item.get(); });
    for (auto const& item : pending) if (item.get() == element) return;
    auto weak = make_weak(element);
    pending.push_back(weak);
    try {
        element.Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low, [weak]() {
            auto button = weak.get();
            try {
                if (button) { DumpWeather(button, L"candidate"); AddGreeting(button); }
            } catch (...) { LogException(L"AddGreeting"); }
            std::erase_if(pending, [&](auto const& item) { auto e=item.get(); return !e || e==button; });
        });
    } catch (...) { pending.pop_back(); throw; }
}
// Scan an observed visual root once, including non-FrameworkElement nodes.
// Descendant notifications still handle templates created later.
void ScanRoot(DependencyObject const& node,unsigned depth,unsigned& budget){
    if(!node||!budget||depth>80)return;
    --budget;
    if(auto e=node.try_as<FrameworkElement>();e&&WeatherButton(get_class_name(e))){
        Log(L"P1 WeatherButton found by root scan");QueueWeather(e);return;
    }
    for(int i=0;i<Media::VisualTreeHelper::GetChildrenCount(node)&&budget;++i)
        ScanRoot(Media::VisualTreeHelper::GetChild(node,i),depth+1,budget);
}
void QueueRoot(DependencyObject const& node,FrameworkElement const& dispatcherSource){
    static thread_local std::vector<weak_ref<DependencyObject>> roots;
    auto root=node;
    for(unsigned i=0;i<80;++i){
        auto parent=Media::VisualTreeHelper::GetParent(root);
        if(!parent)break;
        root=parent;
    }
    for(auto it=roots.begin();it!=roots.end();){
        auto old=it->get();
        if(!old)it=roots.erase(it);
        else{if(old==root)return;++it;}
    }
    if(roots.size()>=128){Log(L"P1 root scan limit reached");return;}
    auto weak=make_weak(root);roots.push_back(weak);
    dispatcherSource.Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,[weak](){
        try{if(auto r=weak.get()){
            unsigned budget=4096;
            Log(L"P1 observed root class="+std::wstring(get_class_name(r).c_str()));
            ScanRoot(r,0,budget);
            if(!budget)Log(L"P1 root scan truncated");
        }}catch(...){LogException(L"P1 root scan");}
    });
}
struct Watcher:implements<Watcher,IVisualTreeServiceCallback,non_agile>{
    com_ptr<IXamlDiagnostics> diag;
    explicit Watcher(com_ptr<IXamlDiagnostics> d):diag(std::move(d)){}
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation,VisualElement e,VisualMutationType mutation) noexcept override{
        if (mutation != Add) return S_OK;
        try {
            // Root and named template notifications suffice. Describe the weather
            // subtree only, rather than walking ancestors for every unrelated icon.
            std::wstring_view type = e.Type ? e.Type : L"";
            std::wstring_view name = e.Name ? e.Name : L"";
            const bool candidate = WeatherButton(type) || layout::weatherGrid(name);
            static thread_local bool observedRoot = false;
            if (observedRoot && relation.Parent && !candidate) return S_OK;
            com_ptr<::IInspectable> raw;
            auto result = diag->GetIInspectableFromHandle(e.Handle, raw.put());
            if (FAILED(result)) { LogHr(L"GetIInspectableFromHandle", result); return S_OK; }
            Windows::Foundation::IInspectable obj{nullptr};
            copy_from_abi(obj, raw.get());
            auto element = obj.try_as<FrameworkElement>();
            if (!element) return S_OK;
            if (candidate) {
                for (DependencyObject node=element; node; node=Media::VisualTreeHelper::GetParent(node)) {
                    if (auto f=node.try_as<FrameworkElement>(); f && WeatherButton(get_class_name(f))) {
                        QueueWeather(f); break;
                    }
                }
            }
            if (!observedRoot || !relation.Parent) { QueueRoot(element,element); observedRoot=true; }
        }catch(...){LogException(L"OnVisualTreeChange");}
        return S_OK;
    }
};
struct Tap:implements<Tap,IObjectWithSite,non_agile>{
    com_ptr<IUnknown> site;
    com_ptr<IVisualTreeService> service;
    com_ptr<Watcher> watcher;
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* value) noexcept override{
        Log(value?L"SetSite(non-null)":L"SetSite(null)");
        try{
            if(service && watcher)LogHr(L"UnadviseVisualTreeChange",service->UnadviseVisualTreeChange(watcher.get()));
            watcher=nullptr;service=nullptr;site=nullptr;
            if(!value){LogHr(L"SetSite",S_OK);return S_OK;}
            site.copy_from(value);
            auto d=site.as<IXamlDiagnostics>();LogHr(L"QueryInterface IXamlDiagnostics",S_OK);
            service=site.as<IVisualTreeService>();LogHr(L"QueryInterface IVisualTreeService",S_OK);
            watcher=make_self<Watcher>(d);
            // Keep module resident while queued UI callbacks exist.
            HMODULE pinned{};
            GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
                reinterpret_cast<LPCWSTR>(&AddGreeting),&pinned);
            auto result=service->AdviseVisualTreeChange(watcher.get());
            LogHr(L"AdviseVisualTreeChange",result);LogHr(L"SetSite",result);return result;
        }catch(...){auto result=to_hresult();LogException(L"SetSite");LogHr(L"SetSite",result);return result;}
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
        try{auto result=make_self<Tap>()->QueryInterface(iid,out);LogHr(L"CreateInstance",result);return result;}catch(...){auto result=to_hresult();LogException(L"CreateInstance");return result;}
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override{return S_OK;}
};
}
STDAPI DllGetClassObject(REFCLSID cls,REFIID iid,void** out){
    if(!out)return E_POINTER;*out=nullptr;
    Log(L"DLL active: DllGetClassObject");
    if(!IsEqualCLSID(cls,TapId))return CLASS_E_CLASSNOTAVAILABLE;
    try{auto result=make_self<Factory>()->QueryInterface(iid,out);LogHr(L"DllGetClassObject",result);return result;}catch(...){auto result=to_hresult();LogException(L"DllGetClassObject");return result;}
}
STDAPI DllCanUnloadNow(){return S_FALSE;}
