#include "../main.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <objidl.h>
#include <gdiplus.h>
#include <shobjidl.h>
using namespace jinju;
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
            throw std::runtime_error(#x);                                                                    \
    } while (false)
int main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
            return 2;
        std::string name = argv[1];
        if (name == "geometry")
        {
            RECT b{0, 1000, 1920, 1080};
            auto centered = StartStrip(b, {700, 1000, 748, 1080});
            CHECK(centered && centered->left == 0 && centered->right == 707);
            auto left = LeftStrip(b, {0, 1000, 48, 1080}, {1700, 1000, 1920, 1080},
                                  {{48, 1000, 96, 1080},
                                   {96, 1000, 144, 1080},
                                   {144, 1000, 192, 1080},
                                   {1700, 1000, 1750, 1080}});
            CHECK(left && left->left == 192 && left->right == 1707);
            auto opened =
                LeftStrip(b, {0, 1000, 48, 1080}, {1700, 1000, 1920, 1080}, {{48, 1000, 240, 1080}});
            CHECK(opened && opened->left == 240 && opened->right == 1707);
            auto closed = LeftStrip(b, {0, 1000, 48, 1080}, {1700, 1000, 1920, 1080}, {});
            CHECK(closed && closed->left == 48 && closed->right == 1707);
            auto narrowLeft =
                LeftStrip(b, {0, 1000, 48, 1080}, {1700, 1000, 1920, 1080}, {{48, 1000, 1700, 1080}});
            CHECK(narrowLeft && narrowLeft->left == 1700 && narrowLeft->right == 1707);
            auto negative = StartStrip({-1920, 1000, 0, 1080}, {-1200, 1000, -1152, 1080});
            CHECK(negative && negative->left == -1920 && negative->right == -1193);
            CHECK(!StartStrip(b, {2000, 1000, 2048, 1080}));
            auto r = FreeStrip(b, {{700, 1000, 1200, 1080}, {1700, 1000, 1920, 1080}}, 7, 0);
            CHECK(r && r->left == 7 && r->right == 693);
            r = FreeStrip(b, {{0, 1000, 600, 1080}, {1700, 1000, 1920, 1080}}, 7, 0);
            CHECK(r && r->left == 607 && r->right == 1693);
            CHECK(!FreeStrip(b, {{0, 1000, 1850, 1080}}, 10, 120));
            CHECK(!FreeStrip(b, {}, 10, 120));
            auto narrow = FreeStrip({0, 0, 100, 40}, {{15, 0, 100, 40}}, 7, 0);
            CHECK(narrow && narrow->left == 7 && narrow->right == 8);
            CHECK(!FreeStrip({0, 0, 100, 40}, {{14, 0, 100, 40}}, 7, 0));
            r = FreeStrip({-1920, 1000, 0, 1080}, {{-1300, 1000, -1000, 1080}, {-200, 1000, 0, 1080}}, 10,
                          120);
            CHECK(r && r->left == -1910 && r->right == -1310);
        }
        else if (name == "animation")
        {
            CHECK(Marquee(300, 100, 15, 45, 100000).x == 15);
            CHECK(Marquee(300, 360, 15, 45, 2000).x == 15);
            CHECK(Marquee(300, 360, 15, 45, 3000).x == -30);
            CHECK(Marquee(300, 360, 15, 45, 4000).x == -75);
            CHECK(Marquee(300, 360, 15, 45, 4600).alpha == 1);
            CHECK(Marquee(300, 360, 15, 45, 4725).alpha == 0.5);
            CHECK(Marquee(300, 360, 15, 45, 4850).alpha == 0);
            CHECK(Marquee(300, 360, 15, 45, 4851).x == 15);
            CHECK(Marquee(300, 360, 15, 45, 5100).alpha == 1);
            CHECK(Crossfade(200) == 0.5 && Crossfade(500) == 1);
        }
        else if (name == "pixels")
        {
            CHECK(WallpaperPixel(0x123456, 0, 1) == 0);
            CHECK(WallpaperPixel(0x123456, 255, 1) == 0xff123456);
            CHECK(WallpaperPixel(0xffffff, 128, 0.5) == 0x40404040);
            CHECK(SameRect({0, 0, 100, 100}, {1, 2, 99, 101}, 2));
            CHECK(!SameRect({0, 0, 100, 100}, {1, 3, 99, 101}, 2));
            Gdiplus::GdiplusStartupInput gi;
            ULONG_PTR token = 0;
            CHECK(Gdiplus::GdiplusStartup(&token, &gi, nullptr) == Gdiplus::Ok);
            std::vector<uint32_t> pixels(16, 0);
            CHECK(RasterWallpaper(pixels.data(), 4, 4, {0, 0, 4, 4}, {0, 0, 4, 4}, L"", DWPOS_FILL,
                                  RGB(31, 97, 201)));
            for (auto p : pixels)
                CHECK((p & 0xffffff) == 0x1f61c9);
            auto file = std::filesystem::temp_directory_path() /
                        ("JinJuWall-" + std::to_string(GetCurrentProcessId()) + ".bmp");
            { // A 2x2 24-bit BMP with four known non-black colors.
                BITMAPFILEHEADER fh{};
                BITMAPINFOHEADER ih{};
                fh.bfType = 0x4d42;
                fh.bfOffBits = sizeof(fh) + sizeof(ih);
                fh.bfSize = fh.bfOffBits + 16;
                ih.biSize = sizeof(ih);
                ih.biWidth = 2;
                ih.biHeight = 2;
                ih.biPlanes = 1;
                ih.biBitCount = 24;
                ih.biSizeImage = 16;
                unsigned char rows[16] = {0, 0, 255, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 0, 0};
                std::ofstream f(file, std::ios::binary);
                f.write(reinterpret_cast<char *>(&fh), sizeof(fh));
                f.write(reinterpret_cast<char *>(&ih), sizeof(ih));
                f.write(reinterpret_cast<char *>(rows), 16);
            }
            std::vector<uint32_t> sample(4);
            CHECK(RasterWallpaper(sample.data(), 2, 2, {0, 0, 2, 2}, {0, 0, 2, 2}, file.wstring(),
                                  DWPOS_STRETCH, RGB(0, 0, 0)));
            CHECK((sample[0] & 0xffffff) == 0x0000ff);
            CHECK((sample[1] & 0xffffff) == 0xffff00);
            CHECK((sample[2] & 0xffffff) == 0xff0000);
            CHECK((sample[3] & 0xffffff) == 0x00ff00);
            uint32_t bottom = 0;
            CHECK(RasterWallpaper(&bottom, 1, 1, {101, 201, 102, 202}, {100, 200, 102, 202}, file.wstring(),
                                  DWPOS_FILL, RGB(0, 0, 0)));
            CHECK((bottom & 0xffffff) == 0x00ff00);
            CHECK((OverlayStyles() & (WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE)) ==
                  (WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE));
            CHECK(TestGlyphComposition());
            std::filesystem::remove(file);
            Gdiplus::GdiplusShutdown(token);
        }
        else if (name == "quotes")
        {
            auto v = ParseLines("\xef\xbb\xbf"
                                "abc\r\n#literal\n\n中文\n");
            CHECK(v.size() == 3 && v[1] == L"#literal" && v[2] == L"中文");
            CHECK(ParseLines("").empty());
            CHECK(ParseLines("\xff").empty());
            CHECK(QuotePath(L"C:\\a.b\\Renamed.exe") == L"C:\\a.b\\Renamed.txt");
            auto dir = std::filesystem::temp_directory_path() /
                       ("JinJuTest-" + std::to_string(GetCurrentProcessId()));
            std::filesystem::create_directory(dir);
            auto p = dir / L"Renamed.txt";
            CHECK(!LoadQuotes(p.wstring()).empty());
            CHECK(std::filesystem::exists(p));
            {
                std::ofstream f(p, std::ios::trunc);
            }
            CHECK(LoadQuotes(p.wstring()).empty());
            CHECK(std::filesystem::file_size(p) == 0);
            {
                std::ofstream f(p);
                f << "#literal\n";
            }
            CHECK(LoadQuotes(p.wstring()) == std::vector<std::wstring>{L"#literal"});
            std::filesystem::remove_all(dir);
        }
        else
            return 2;
        std::cout << name << " passed\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
