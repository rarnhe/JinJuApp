#pragma once
#include <cmath>
#include <string_view>
namespace layout {
inline bool positive(double value){return std::isfinite(value)&&value>0;}
inline bool validSize(double w,double h){return positive(w)&&positive(h);}
inline bool inside(double x,double y,double w,double h,double outerW,double outerH){
    constexpr double epsilon=0.5;
    return validSize(w,h)&&validSize(outerW,outerH)&&std::isfinite(x)&&std::isfinite(y)&&
        x>=-epsilon&&y>=-epsilon&&x+w<=outerW+epsilon&&y+h<=outerH+epsilon;
}
inline bool insideRect(double x,double y,double w,double h,double left,double top,double width,double height){
    return std::isfinite(left)&&std::isfinite(top)&&inside(x-left,y-top,w,h,width,height);
}
inline bool canExpand(double panel,double content,double extra,double button){
    return positive(panel)&&positive(content)&&positive(extra)&&positive(button)&&
           content<=panel&&panel+extra<=button+0.5;
}
inline bool preservesColumn(unsigned count,int column,int span){
    const auto effective=count?count:1u;
    return column>=0&&span>0&&static_cast<unsigned>(column)<effective&&
        static_cast<unsigned>(span)<=effective-static_cast<unsigned>(column);
}
inline constexpr bool weatherGrid(std::wstring_view name){return name==L"AugmentedEntryPointContentGrid";}
}
