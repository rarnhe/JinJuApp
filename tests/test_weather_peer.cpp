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
    if (!attached.install(first,weather) || !attached.intact(button)) return 10;
    auto original=attached.text();
    if (attached.install(first,weather) || attached.text()!=original) return 11;

    first.Children().RemoveAt(0); // Same panel, new weather ContentGrid.
    first.Children().Append(replacement);
    if (attached.intact(button) || !attached.install(first,replacement)) return 12;
    uint32_t index{};
    if (first.Children().IndexOf(original,index) || !attached.intact(button)) return 13;

    auto removed=attached.text();
    if (!first.Children().IndexOf(removed,index)) return 14;
    first.Children().RemoveAt(index); // Only the greeting was recycled.
    if (attached.intact(button) || !attached.install(first,replacement) || first.Children().Size()!=2) return 15;

    button.Children().Clear(); // Entire root panel was rebuilt.
    button.Children().Append(second);
    second.Children().Append(weather);
    if (attached.intact(button) || !attached.install(second,weather) ||
        first.Children().Size()!=1 || !attached.intact(button)) return 16;
    for (int i=0;i<20;++i)
        if (attached.install(second,weather) || second.Children().Size()!=2) return 17;
    return 0;
}

int main(int argc, char** argv) {
    using namespace greeting;
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    try {
        auto manager=Hosting::WindowsXamlManager::InitializeForCurrentThread();
        if (argc==2 && std::string_view(argv[1])=="rebuild") {
            auto result=testRebuild();
            if (result) std::cerr<<"Template rebuild assertion "<<result<<" failed\n";
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
