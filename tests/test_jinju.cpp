#include "../main.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdexcept>
using namespace jinju;
#define CHECK(x) do{if(!(x))throw std::runtime_error(#x);}while(false)
int main(int argc,char** argv){
 try{
 if(argc!=2)return 2;std::string name=argv[1];
 if(name=="geometry"){
  RECT b{0,1000,1920,1080};
  auto r=FreeStrip(b,{{700,1000,1200,1080},{1700,1000,1920,1080}},10,120);CHECK(r&&r->left==10&&r->right==690);
  r=FreeStrip(b,{{0,1000,600,1080},{1700,1000,1920,1080}},10,120);CHECK(r&&r->left==610&&r->right==1690);
  CHECK(!FreeStrip(b,{{0,1000,1850,1080}},10,120));CHECK(!FreeStrip(b,{},10,120));
  r=FreeStrip({-1920,1000,0,1080},{{-1300,1000,-1000,1080},{-200,1000,0,1080}},10,120);CHECK(r&&r->left==-1910&&r->right==-1310);
 }else if(name=="animation"){
  CHECK(Marquee(300,100,15,45,100000).x==15);CHECK(Marquee(300,360,15,45,2000).x==15);
  CHECK(Marquee(300,360,15,45,3000).x==-30);CHECK(Marquee(300,360,15,45,4000).x==-75);
  CHECK(Marquee(300,360,15,45,4600).alpha==1);CHECK(Marquee(300,360,15,45,4725).alpha==0.5);
  CHECK(Marquee(300,360,15,45,4850).alpha==0);CHECK(Marquee(300,360,15,45,4851).x==15);
  CHECK(Marquee(300,360,15,45,5100).alpha==1);CHECK(Crossfade(200)==0.5&&Crossfade(500)==1);
 }else if(name=="pixels"){
  CHECK(WallpaperPixel(0x123456,0,1)==0);CHECK(WallpaperPixel(0x123456,255,1)==0xff123456);
  CHECK(WallpaperPixel(0xffffff,128,0.5)==0x40404040);
  CHECK(SameRect({0,0,100,100},{1,2,99,101},2));CHECK(!SameRect({0,0,100,100},{1,3,99,101},2));
 }else if(name=="quotes"){
  auto v=ParseLines("\xef\xbb\xbf" "abc\r\n#literal\n\n中文\n");CHECK(v.size()==3&&v[1]==L"#literal"&&v[2]==L"中文");
  CHECK(ParseLines("").empty());CHECK(ParseLines("\xff").empty());CHECK(QuotePath(L"C:\\a.b\\Renamed.exe")==L"C:\\a.b\\Renamed.txt");
  auto dir=std::filesystem::temp_directory_path()/("JinJuTest-"+std::to_string(GetCurrentProcessId()));std::filesystem::create_directory(dir);auto p=dir/L"Renamed.txt";
  CHECK(!LoadQuotes(p.wstring()).empty());CHECK(std::filesystem::exists(p));{std::ofstream f(p,std::ios::trunc);}
  CHECK(LoadQuotes(p.wstring()).empty());CHECK(std::filesystem::file_size(p)==0);{std::ofstream f(p);f<<"#literal\n";}
  CHECK(LoadQuotes(p.wstring())==std::vector<std::wstring>{L"#literal"});std::filesystem::remove_all(dir);
 }else return 2;
 std::cout<<name<<" passed\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
