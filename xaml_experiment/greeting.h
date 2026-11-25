#pragma once
#include "shared.h"
#include "layout_policy.h"
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace greeting {
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
inline bool hasMarker(Panel const& parent) {
    for (auto const& child : parent.Children())
        if (auto e=child.try_as<FrameworkElement>(); e && e.Name()==Marker) return true;
    return false;
}
inline TextBlock appendPeer(Panel const& panel, Grid const& weather) {
    if (hasMarker(panel) || hasMarker(weather)) return nullptr;
    TextBlock text;
    text.Name(Marker);
    text.Text(Greeting);
    text.TextWrapping(TextWrapping::NoWrap);
    text.HorizontalAlignment(HorizontalAlignment::Left);
    text.VerticalAlignment(VerticalAlignment::Center);
    text.IsHitTestVisible(false);
    text.Opacity(0);
    text.RenderTransform(Media::TranslateTransform{});
    panel.Children().Append(text);
    return text;
}
class Attachment {
public:
    bool install(Panel const& panel, Grid const& weather) {
        if (auto existing=parent.get(); existing && existing==panel && content.get()==weather &&
            present(panel,weather)) return false;
        remove();
        auto text=appendPeer(panel,weather);
        parent=winrt::make_weak(panel);
        content=winrt::make_weak(weather);
        if (text) peer=winrt::make_weak(text);
        owned=static_cast<bool>(text);
        return owned;
    }
    bool intact(FrameworkElement const& button) const {
        auto panel=parent.get();
        auto grid=content.get();
        if (!panel || !grid || Media::VisualTreeHelper::GetParent(panel)!=button) return false;
        uint32_t index{};
        return panel.Children().IndexOf(grid,index) && present(panel,grid);
    }
    void remove() {
        if (auto text=peer.get()) {
            auto p=Media::VisualTreeHelper::GetParent(text);
            auto panel=p ? p.try_as<Panel>() : parent.get();
            uint32_t index{};
            if (panel && panel.Children().IndexOf(text,index)) panel.Children().RemoveAt(index);
        }
        peer={}; parent={}; content={}; owned=false;
    }
    TextBlock text() const { return peer.get(); }
    Panel panel() const { return parent.get(); }
    Grid grid() const { return content.get(); }
private:
    winrt::weak_ref<Panel> parent;
    winrt::weak_ref<Grid> content;
    winrt::weak_ref<TextBlock> peer;
    bool owned=false;
    bool present(Panel const& panel, Grid const& grid) const {
        if (!owned) return hasMarker(panel) || hasMarker(grid);
        auto text=peer.get();
        uint32_t index{};
        return text && panel.Children().IndexOf(text,index);
    }
};
inline layout::Box bounds(FrameworkElement const& element, UIElement const& relative) {
    winrt::Windows::Foundation::Rect local{0,0,static_cast<float>(element.ActualWidth()),
                                              static_cast<float>(element.ActualHeight())};
    auto r=element.TransformToVisual(relative).TransformBounds(local);
    return {r.X,r.Y,r.Width,r.Height};
}
enum class Placement { pending, visible, noSpace };
inline Placement position(TextBlock const& text, Panel const& panel, Grid const& weather,
                          FrameworkElement const& button) {
    auto hide=[&](Placement state) { if (text.Opacity()!=0) text.Opacity(0); return state; };
    const auto current=bounds(text,panel);
    if (!layout::validSize(current.width,current.height)) return hide(Placement::pending);
    const auto outer=bounds(button,panel);
    const auto content=bounds(weather,panel);
    auto wanted=layout::besideWeather(outer,content,current.width,current.height);
    if (!wanted) return hide(Placement::noSpace);
    // Account for explicit clips that apply to the peer, in the same coordinates.
    for (DependencyObject n=panel; n; n=Media::VisualTreeHelper::GetParent(n)) {
        if (auto ui=n.try_as<UIElement>()) {
            if (auto clip=ui.Clip()) {
                auto r=clip.Bounds();
                r=ui.TransformToVisual(panel).TransformBounds(r);
                if (!layout::fits(*wanted,{r.X,r.Y,r.Width,r.Height})) return hide(Placement::noSpace);
            }
        }
        if (n==button) break;
    }
    auto move=text.RenderTransform().as<Media::TranslateTransform>();
    const double dx=wanted->x-current.x, dy=wanted->y-current.y;
    if (std::abs(dx)>0.125) move.X(move.X()+dx);
    if (std::abs(dy)>0.125) move.Y(move.Y()+dy);
    const auto actual=bounds(text,panel);
    if (!layout::fits(actual,outer) || actual.x<content.x+content.width-0.5 ||
        std::abs(actual.x-wanted->x)>0.5 || std::abs(actual.y-wanted->y)>0.5)
        return hide(Placement::pending);
    if (text.Opacity()!=1) text.Opacity(1);
    return Placement::visible;
}
}
