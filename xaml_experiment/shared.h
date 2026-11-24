#pragma once
#include <windows.h>
#include <string_view>
inline constexpr CLSID TapId={0x41e438b1,0x7914,0x4c6c,{0x92,0x4a,0x3f,0xb9,0x73,0x11,0x5e,0x82}};
inline constexpr wchar_t Greeting[]=L"你好。";
inline constexpr wchar_t Marker[]=L"JinJuWeatherHello";
inline constexpr bool WeatherButton(std::wstring_view name){return name==L"Taskbar.AugmentedEntryPointButton";}
