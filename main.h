#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
namespace jinju {
struct Frame { int x; double alpha; };
Frame Marquee(int view,int text,int pad,double speed,double ms);
double Crossfade(double ms);
std::optional<RECT> FreeStrip(RECT bar,std::vector<RECT> occupied,int margin,int minimum);
std::vector<std::wstring> ParseLines(const std::string& bytes);
std::wstring GetProgramName();
std::wstring QuotePath(const std::wstring& exe);
std::vector<std::wstring> LoadQuotes(const std::wstring& path);
uint32_t WallpaperPixel(uint32_t rgb,unsigned coverage,double opacity);
bool RasterWallpaper(uint32_t* pixels, int width, int height, RECT strip, RECT area, const std::wstring& file, int position, COLORREF background);
bool SameRect(const RECT& a,const RECT& b,int tolerance=0);
}
