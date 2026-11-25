#include "../xaml_experiment/greeting.h"
#include <winrt/Windows.UI.Xaml.Hosting.h>
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

int main(int argc, char** argv) {
    using namespace greeting;
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    try {
        auto manager=Hosting::WindowsXamlManager::InitializeForCurrentThread();
        if (argc==2) {
            auto result=std::string_view(argv[1])=="rebuild" ? testRebuild() : testRemoval();
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
