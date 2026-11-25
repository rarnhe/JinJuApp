#include "../xaml_experiment/greeting.h"
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <iostream>

int main() {
    using namespace greeting;
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    try {
        auto manager=Hosting::WindowsXamlManager::InitializeForCurrentThread();
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
