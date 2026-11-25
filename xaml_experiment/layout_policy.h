#pragma once
#include <cmath>
#include <string_view>
#include <optional>
namespace layout {
inline bool positive(double value){return std::isfinite(value)&&value>0;}
inline bool validSize(double w,double h){return positive(w)&&positive(h);}
inline bool inside(double x,double y,double w,double h,double outerW,double outerH){
    constexpr double epsilon=0.5;
    return validSize(w,h)&&validSize(outerW,outerH)&&std::isfinite(x)&&std::isfinite(y)&&
        x>=-epsilon&&y>=-epsilon&&x+w<=outerW+epsilon&&y+h<=outerH+epsilon;
}
struct Box { double x, y, width, height; };
inline bool fits(Box item, Box outer) {
    return inside(item.x-outer.x,item.y-outer.y,item.width,item.height,outer.width,outer.height);
}
inline std::optional<Box> besideWeather(Box button, Box weather, double width, double height) {
    if (!fits(weather,button)) return {};
    constexpr double gap=6;
    Box desired{weather.x+weather.width+gap,button.y+(button.height-height)/2,width,height};
    if (!fits(desired,button)) return {};
    return desired;
}
inline constexpr bool weatherGrid(std::wstring_view name){return name==L"AugmentedEntryPointContentGrid";}
}
