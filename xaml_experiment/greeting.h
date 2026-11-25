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
