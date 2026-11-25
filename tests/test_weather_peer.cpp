#include "../xaml_experiment/greeting.h"
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <iostream>
#include <string_view>

int testRebuild() {
    using namespace greeting;
    Grid button, first, weather, replacement, second;
    button.Children().Append(first);
    first.Children().Append(weather);
    Attachment attached;
    auto intact=[&](Grid const& expectedWeather) {
        auto current=button.Children().GetAt(0).as<Panel>();
        return attached.intact(current,expectedWeather);
    };
    if (!attached.install(first,weather)) return 10;
    if (!intact(weather)) return 18;
    auto original=attached.text();
    if (attached.install(first,weather) || attached.text()!=original) return 11;

    first.Children().RemoveAt(0); // Same panel, new weather ContentGrid.
    first.Children().Append(replacement);
    if (intact(replacement) || !attached.install(first,replacement)) return 12;
    uint32_t index{};
    if (first.Children().IndexOf(original,index) || !intact(replacement)) return 13;

    auto removed=attached.text();
    if (!first.Children().IndexOf(removed,index)) return 14;
    first.Children().RemoveAt(index); // Only the greeting was recycled.
    if (intact(replacement) || !attached.install(first,replacement) || first.Children().Size()!=2) return 15;

    button.Children().Clear(); // Entire root panel was rebuilt.
    button.Children().Append(second);
    second.Children().Append(weather);
    if (intact(weather) || !attached.install(second,weather) ||
        first.Children().Size()!=1 || !intact(weather)) return 16;
    for (int i=0;i<20;++i)
        if (attached.install(second,weather) || second.Children().Size()!=2) return 17;
    return 0;
}

int testRemoval() {
    using namespace greeting;
    Grid panel, weather;
    Border card;
    ColumnDefinition column;
    weather.Children().Append(card);
    weather.ColumnDefinitions().Append(column);
    panel.Children().Append(weather);
    Attachment owned;
    if (!owned.install(panel,weather)) return 20;
    // An element with an identical name must survive removal of our instance.
    TextBlock foreign;
    foreign.Name(Marker);
    panel.Children().Append(foreign);
    owned.remove();
    owned.remove();
    if (panel.Children().Size()!=2 || panel.Children().GetAt(0)!=weather ||
        panel.Children().GetAt(1)!=foreign || weather.Children().GetAt(0)!=card ||
        weather.ColumnDefinitions().GetAt(0)!=column) return 21;
    if (owned.install(panel,weather)) return 22;
    owned.remove();
    return panel.Children().Size()==2 && panel.Children().GetAt(1)==foreign ? 0 : 23;
}

int testHostedRemoval() {
    using namespace greeting;
    auto window=CreateWindowExW(0,L"STATIC",L"JinJu XAML layout test",WS_POPUP,0,0,320,80,
                                nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    winrt::check_bool(window!=nullptr);
    Hosting::DesktopWindowXamlSource island;
    auto native=island.as<IDesktopWindowXamlSourceNative>();
    winrt::check_hresult(native->AttachToWindow(window));
    HWND child{};
    winrt::check_hresult(native->get_WindowHandle(&child));
    winrt::check_bool(SetWindowPos(child,nullptr,0,0,320,80,SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW));
    Grid button,panel,weather;
    button.Width(216); button.Height(48);
    panel.Width(152); panel.Height(48); panel.HorizontalAlignment(HorizontalAlignment::Center);
    weather.Width(136); weather.Height(40); weather.HorizontalAlignment(HorizontalAlignment::Center);
    weather.VerticalAlignment(VerticalAlignment::Center);
    panel.Children().Append(weather);
    button.Children().Append(panel);
    island.Content(button);
    auto layoutAt=[&](float width) {
        button.Width(width);
        button.Measure({width,48});
        button.Arrange({0,0,width,48});
        button.UpdateLayout();
    };
    layoutAt(216);
    const auto before=bounds(weather,button);
    Attachment attachment;
    if (!attachment.install(panel,weather)) return 30;
    auto text=attachment.text();
    layoutAt(216);
    auto placement=position(text,panel,weather,button);
    const auto shown=bounds(text,button);
    const auto during=bounds(weather,button);
    std::cout<<"Weather x="<<during.x<<" width="<<during.width<<", peer x="<<shown.x
             <<" width="<<shown.width<<", placement="<<static_cast<int>(placement)<<"\n";
    if (placement!=Placement::visible || text.Opacity()!=1 || text.IsHitTestVisible() ||
        shown.x<during.x+during.width+1.5 || !layout::fits(shown,bounds(button,button))) return 31;
    layoutAt(152);
    if (position(text,panel,weather,button)!=Placement::noSpace || text.Opacity()!=0) return 32;
    layoutAt(216);
    if (position(text,panel,weather,button)!=Placement::visible) return 33;
    attachment.remove(); layoutAt(216);
    const auto after=bounds(weather,button);
    auto unchanged=[&](layout::Box b) {
        return std::abs(b.x-before.x)<0.5 && std::abs(b.y-before.y)<0.5 &&
               std::abs(b.width-before.width)<0.5 && std::abs(b.height-before.height)<0.5;
    };
    const bool restored=unchanged(during) && unchanged(after) && panel.Children().Size()==1 &&
                        panel.Children().GetAt(0)==weather;
    island.Close();
    DestroyWindow(window);
    return restored ? 0 : 34;
}

int main(int argc, char** argv) {
    using namespace greeting;
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    try {
        auto manager=Hosting::WindowsXamlManager::InitializeForCurrentThread();
        if (argc==2) {
            const std::string_view scenario=argv[1];
            int result;
            if (scenario=="rebuild") result=testRebuild();
            else if (scenario=="remove") result=testRemoval();
            else if (scenario=="window") result=testHostedRemoval();
            else return 4;
            if (result) std::cerr<<"Weather lifecycle assertion "<<result<<" failed\n";
            manager.Close();
            return result;
        }
        {
            Grid panel, weather;
            ColumnDefinition column;
            weather.ColumnDefinitions().Append(column);
            Border card;
            weather.Children().Append(card);
            panel.Children().Append(weather);
            auto peer=appendPeer(panel,weather);
            if (!peer || panel.Children().Size()!=2 || panel.Children().GetAt(0)!=weather ||
                panel.Children().GetAt(1)!=peer || weather.Children().Size()!=1 ||
                weather.Children().GetAt(0)!=card || weather.ColumnDefinitions().Size()!=1 ||
                weather.ColumnDefinitions().GetAt(0)!=column || peer.IsHitTestVisible() ||
                peer.Text()!=Greeting || peer.Opacity()!=0) return 1;
            if (appendPeer(panel,weather) || panel.Children().Size()!=2) return 2;
        }
        manager.Close();
    } catch (winrt::hresult_error const& e) {
        std::wcerr << L"Local XAML host: " << std::hex << e.code().value << L" " << e.message().c_str() << L"\n";
        return 3;
    }
}
