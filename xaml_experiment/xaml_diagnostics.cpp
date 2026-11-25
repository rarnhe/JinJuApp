#include "shared.h"
#include "async_log.h"
#include <algorithm>
#include "weather_state.h"
#include "greeting.h"
#include "cleanup_runtime.h"
#include <memory>
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
struct WeatherParts {
    Panel panel{nullptr};
    Grid grid{nullptr};
};
WeatherParts FindWeatherParts(FrameworkElement const& button) {
    for (int i=0; i<Media::VisualTreeHelper::GetChildrenCount(button); ++i) {
        auto panel=Media::VisualTreeHelper::GetChild(button,i).try_as<Panel>();
        if (!panel || panel.Name()!=L"ExperienceToggleButtonRootPanel") continue;
        for (auto const& child : panel.Children())
            if (auto g=child.try_as<Grid>(); g && layout::weatherGrid(g.Name())) return {panel,g};
    }
    return {};
}
void CountCards(DependencyObject const& node, unsigned depth, unsigned& budget, layout::WeatherSample& sample) {
    if (!budget || depth>24) return;
    --budget;
    if (get_class_name(node)==L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel") {
        ++sample.cards;
        if (auto card=node.try_as<FrameworkElement>(); card && card.Visibility()==Visibility::Visible &&
            layout::validSize(card.ActualWidth(),card.ActualHeight())) ++sample.measuredCards;
        return; // Nested WholeItemsPanels belong to the same card.
    }
    for (int i=0; i<Media::VisualTreeHelper::GetChildrenCount(node) && budget; ++i)
        CountCards(Media::VisualTreeHelper::GetChild(node,i),depth+1,budget,sample);
}
layout::WeatherSample SampleWeather(FrameworkElement const& button, WeatherParts const& parts) {
    if (!parts.grid) return {};
    auto const& g=parts.grid;
    layout::WeatherSample result{button.ActualWidth(),button.ActualHeight(),
        parts.panel.ActualWidth(),parts.panel.ActualHeight(),g.ActualWidth(),g.ActualHeight(),
        g.Children().Size(),g.RowDefinitions().Size(),g.ColumnDefinitions().Size(),0,0};
    unsigned budget=256;
    CountCards(g,0,budget,result);
    if (!budget) return {};
    return result;
}
class WeatherWatch : public std::enable_shared_from_this<WeatherWatch> {
public:
    WeatherWatch(FrameworkElement const& target,std::shared_ptr<cleanup::Runtime> const& owner)
        : button(make_weak(target)), runtime(owner) {}
    weak_ref<FrameworkElement> button;

    void listen(FrameworkElement const& target) {
        auto weak=weak_from_this();
        timer.Interval(std::chrono::milliseconds(250));
        tickToken=timer.Tick([weak](auto const&, auto const&) {
            if (auto watch=weak.lock()) {
                try { watch->tick(); } catch (...) {
                    watch->stability.reset();
                    watch->pause();
                    LogException(L"Weather layout tick");
                }
            }
        });
        layoutToken=target.LayoutUpdated([weak](auto const&, auto const&) {
            try { if (auto watch=weak.lock()) watch->layoutChanged(); }
            catch (...) { LogException(L"Weather layout update"); }
        });
        unloadToken=target.Unloaded([weak](auto const&, auto const&) {
            try { if (auto watch=weak.lock()) {
                watch->suspended=true;
                watch->resetLayout();
                Log(L"Weather button unloaded");
            }} catch (...) { LogException(L"Weather button unload"); }
        });
        loadToken=target.Loaded([weak](auto const&, auto const&) {
            try { if (auto watch=weak.lock()) { watch->suspended=false; watch->start(); } }
            catch (...) { LogException(L"Weather button load"); }
        });
        start();
    }
    void close() {
        suspended=true;
        if (auto target=button.get()) {
            if (layoutToken.value) { target.LayoutUpdated(layoutToken); layoutToken={}; }
            if (loadToken.value) { target.Loaded(loadToken); loadToken={}; }
            if (unloadToken.value) { target.Unloaded(unloadToken); unloadToken={}; }
        }
        if (tickToken.value) { timer.Tick(tickToken); tickToken={}; }
        resetLayout();
    }
    void start() {
        if (!active()) return;
        if (!suspended && !running && !installed && !changing) {
            timer.Start();
            running=true;
        }
    }
private:
    std::weak_ptr<cleanup::Runtime> runtime;
    bool active() const {
        auto owner=runtime.lock();
        return owner && !owner->stopping();
    }
    DispatcherTimer timer;
    event_token tickToken{}, layoutToken{}, unloadToken{}, loadToken{};
    weak_ref<Grid> sampledGrid;
    weak_ref<Panel> sampledPanel;
    greeting::Attachment attachment;
    layout::Stability stability;
    std::optional<greeting::Placement> lastPlacement;
    std::optional<layout::Readiness> lastState;
    bool running=false, installed=false, suspended=false, changing=false;
    struct Change {
        bool& flag;
        explicit Change(bool& changing) : flag(changing) { flag=true; }
        ~Change() { flag=false; }
    };

    void pause() { timer.Stop(); running=false; }
    void resetLayout() {
        pause();
        installed=false;
        stability.reset();
        sampledGrid={}; sampledPanel={};
        lastState.reset(); lastPlacement.reset();
        attachment.remove();
    }
    void updatePosition(FrameworkElement const& target) {
        auto text=attachment.text();
        if (!text) return;
        try {
            auto result=greeting::position(text,attachment.panel(),attachment.grid(),target);
            if (lastPlacement==result) return;
            lastPlacement=result;
            switch (result) {
            case greeting::Placement::pending: Log(L"Weather peer awaiting layout"); break;
            case greeting::Placement::noSpace: Log(L"Weather peer hidden: insufficient clear space"); break;
            case greeting::Placement::visible: Log(L"Weather peer fits beside content; visualProof=manual"); break;
            }
        } catch (...) { text.Opacity(0); LogException(L"Weather peer placement"); }
    }
    void layoutChanged() {
        if (!active() || suspended || changing) return;
        if (!installed) { start(); return; }
        auto target=button.get();
        if (!target) return;
        {
            Change guard(changing);
            auto current=FindWeatherParts(target);
            if (attachment.intact(current.panel,current.grid)) { updatePosition(target); return; }
            resetLayout();
            Log(L"Weather template or peer replaced; waiting for stable layout");
        }
        start();
    }
    void tick() {
        auto target=button.get();
        if (!active() || !target || suspended || installed) { pause(); return; }
        if (changing) return;
        Change guard(changing);
        auto parts=FindWeatherParts(target);
        if (sampledGrid.get()!=parts.grid || sampledPanel.get()!=parts.panel) {
            stability.reset();
            sampledGrid=parts.grid ? make_weak(parts.grid) : weak_ref<Grid>{};
            sampledPanel=parts.panel ? make_weak(parts.panel) : weak_ref<Panel>{};
        }
        auto state=stability.observe(SampleWeather(target,parts));
        if (lastState!=state) {
            lastState=state;
            switch (state) {
            case layout::Readiness::waiting: Log(L"Weather waiting for measured cards"); break;
            case layout::Readiness::measuring: Log(L"Weather measuring stable layout"); break;
            case layout::Readiness::ready: Log(L"Weather layout stable (4 samples)"); break;
            }
        }
        if (state==layout::Readiness::waiting) { pause(); return; }
        if (state==layout::Readiness::ready) {
            pause();
            DumpWeather(target,L"stable");
            bool added=attachment.install(parts.panel,parts.grid);
            installed=true;
            Log(added ? L"Weather peer appended beside ContentGrid" : L"Weather peer skipped: marker already present");
            updatePosition(target);
        }
    }
};
struct ThreadWeather {
    std::vector<std::shared_ptr<WeatherWatch>> watches;
    std::vector<weak_ref<DependencyObject>> roots;
    bool observedRoot=false;
};
ThreadWeather& LocalWeather() {
    static thread_local ThreadWeather state;
    return state;
}
Windows::Foundation::IAsyncAction ClearDispatcher(Windows::UI::Core::CoreDispatcher dispatcher) {
    HRESULT cleared=E_PENDING;
    co_await dispatcher.RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,[&cleared]() {
        auto& state=LocalWeather();
        cleared=S_OK;
        std::erase_if(state.watches,[&](auto const& watch) {
            try { watch->close(); return true; }
            catch (...) { cleared=to_hresult(); LogException(L"Weather teardown"); return false; }
        });
        state.roots.clear();
        state.observedRoot=false;
    });
    LogHr(L"Weather UI cleanup",cleared);
    check_hresult(cleared);
}
void QueueWeather(FrameworkElement const& target,std::shared_ptr<cleanup::Runtime> const& runtime) {
    if (runtime->stopping()) return;
    auto& watches=LocalWeather().watches;
    std::erase_if(watches, [](auto const& watch) { return !watch->button.get(); });
    for (auto const& watch : watches)
        if (watch->button.get()==target) { watch->start(); return; }
    auto watch=std::make_shared<WeatherWatch>(target,runtime);
    watches.push_back(watch);
    try { watch->listen(target); }
    catch (...) { watch->close(); std::erase(watches,watch); throw; }
}
// Scan an observed visual root once, including non-FrameworkElement nodes.
// Descendant notifications still handle templates created later.
void ScanRoot(DependencyObject const& node,unsigned depth,unsigned& budget,std::shared_ptr<cleanup::Runtime> const& runtime){
    if(!node||!budget||depth>80)return;
    --budget;
    if(auto e=node.try_as<FrameworkElement>();e&&WeatherButton(get_class_name(e))){
        Log(L"P1 WeatherButton found by root scan");QueueWeather(e,runtime);return;
    }
    for(int i=0;i<Media::VisualTreeHelper::GetChildrenCount(node)&&budget;++i)
        ScanRoot(Media::VisualTreeHelper::GetChild(node,i),depth+1,budget,runtime);
}
void QueueRoot(DependencyObject const& node,FrameworkElement const& dispatcherSource,
               std::shared_ptr<cleanup::Runtime> const& runtime){
    auto& roots=LocalWeather().roots;
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
    dispatcherSource.Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,[weak,owner=std::weak_ptr(runtime)](){
        auto runtime=owner.lock();
        if (!runtime || runtime->stopping()) return;
        try{if(auto r=weak.get()){
            unsigned budget=4096;
            Log(L"P1 observed root class="+std::wstring(get_class_name(r).c_str()));
            ScanRoot(r,0,budget,runtime);
            if(!budget)Log(L"P1 root scan truncated");
        }}catch(...){LogException(L"P1 root scan");}
    });
}
void ObserveElement(FrameworkElement const& element,bool candidate,bool rootEvent,
                    std::shared_ptr<cleanup::Runtime> const& runtime) {
    if (runtime->stopping()) return;
    auto& seen=LocalWeather().observedRoot;
    if (seen && !rootEvent && !candidate) return;
    auto dispatcher=element.Dispatcher();
    if (!runtime->trackUi(GetCurrentThreadId(),[dispatcher] { return ClearDispatcher(dispatcher); })) return;
    if (candidate) {
        for (DependencyObject node=element; node; node=Media::VisualTreeHelper::GetParent(node)) {
            if (auto f=node.try_as<FrameworkElement>(); f && WeatherButton(get_class_name(f))) {
                QueueWeather(f,runtime); break;
            }
        }
    }
    if (!seen || rootEvent) { QueueRoot(element,element,runtime); seen=true; }
}
struct Watcher:implements<Watcher,IVisualTreeServiceCallback,non_agile>{
    com_ptr<IXamlDiagnostics> diag;
    std::weak_ptr<cleanup::Runtime> owner;
    Watcher(com_ptr<IXamlDiagnostics> d,std::shared_ptr<cleanup::Runtime> const& runtime)
        :diag(std::move(d)),owner(runtime){}
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation,VisualElement e,VisualMutationType mutation) noexcept override{
        auto runtime=owner.lock();
        if (!runtime || runtime->stopping() || mutation!=Add) return S_OK;
        try {
            std::wstring_view type=e.Type ? e.Type : L"";
            std::wstring_view name=e.Name ? e.Name : L"";
            const bool candidate=WeatherButton(type) || layout::weatherGrid(name);
            const bool rootEvent=relation.Parent==0;
            if (LocalWeather().observedRoot && !rootEvent && !candidate) return S_OK;
            com_ptr<::IInspectable> raw;
            auto hr=diag->GetIInspectableFromHandle(e.Handle,raw.put());
            if (FAILED(hr)) { LogHr(L"GetIInspectableFromHandle",hr); return S_OK; }
            Windows::Foundation::IInspectable obj{nullptr};
            copy_from_abi(obj,raw.get());
            auto element=obj.try_as<FrameworkElement>();
            if (!element) return S_OK;
            auto dispatcher=element.Dispatcher();
            if (dispatcher.HasThreadAccess()) {
                ObserveElement(element,candidate,rootEvent,runtime);
            } else {
                dispatcher.RunAsync(Windows::UI::Core::CoreDispatcherPriority::Low,
                    [weak=make_weak(element),owner=owner,candidate,rootEvent] {
                        try {
                            if (auto active=owner.lock(); active && !active->stopping())
                                if (auto target=weak.get()) ObserveElement(target,candidate,rootEvent,active);
                        } catch (...) { LogException(L"Observe dispatched layout"); }
                    });
            }
        } catch (...) { LogException(L"OnVisualTreeChange"); }
        return S_OK;
    }
};
struct Binding {
    apartment_context home;
    std::mutex mutex;
    com_ptr<IUnknown> site;
    com_ptr<IVisualTreeService> service;
    com_ptr<Watcher> watcher;
    bool advised=false;

    HRESULT detach() {
        std::lock_guard lock(mutex);
        if (advised) {
            auto result=service->UnadviseVisualTreeChange(watcher.get());
            LogHr(L"UnadviseVisualTreeChange",result);
            if (FAILED(result)) return result;
        }
        advised=false; watcher=nullptr; service=nullptr; site=nullptr;
        return S_OK;
    }
    static Windows::Foundation::IAsyncAction closeOnOwner(std::shared_ptr<Binding> binding) {
        co_await binding->home;
        check_hresult(binding->detach());
    }
};
struct Tap:implements<Tap,IObjectWithSite,non_agile>{
    std::shared_ptr<Binding> binding;
    std::shared_ptr<cleanup::Runtime> runtime;
    HRESULT release(bool closeWhenEmpty) {
        if (!binding) return S_OK;
        auto hr=binding->detach();
        if (FAILED(hr)) return hr;
        if (runtime) runtime->forget(reinterpret_cast<uintptr_t>(binding.get()),closeWhenEmpty);
        binding.reset(); runtime.reset();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* value) noexcept override{
        Log(value?L"SetSite(non-null)":L"SetSite(null)");
        try{
            check_hresult(release(!value));
            if (!value) { LogHr(L"SetSite",S_OK); return S_OK; }
            binding=std::make_shared<Binding>();
            binding->site.copy_from(value);
            auto d=binding->site.as<IXamlDiagnostics>();
            binding->service=binding->site.as<IVisualTreeService>();
            HMODULE pinned{};
            check_bool(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
                reinterpret_cast<LPCWSTR>(&QueueWeather),&pinned));
            {
                std::lock_guard lock(binding->mutex);
                runtime=cleanup::Runtime::subscribe(reinterpret_cast<uintptr_t>(binding.get()),
                    [b=binding] { return Binding::closeOnOwner(b); });
                binding->watcher=make_self<Watcher>(d,runtime);
                auto result=binding->service->AdviseVisualTreeChange(binding->watcher.get());
                LogHr(L"AdviseVisualTreeChange",result);
                check_hresult(result);
                binding->advised=true;
            }
            LogHr(L"SetSite",S_OK); return S_OK;
        } catch (...) {
            auto result=to_hresult();
            LogException(L"SetSite");
            release(true);
            return result;
        }
    }
    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid,void** out) noexcept override{
        if(!out)return E_POINTER;
        *out=nullptr;
        if (!binding) return E_FAIL;
        std::lock_guard lock(binding->mutex);
        return binding->site ? binding->site->QueryInterface(iid,out) : E_FAIL;
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
