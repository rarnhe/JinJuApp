#pragma once
#include <windows.h>
#include <string>
#include <string_view>
#include <filesystem>
#include <sstream>
#include <iomanip>

namespace diagnostics {
inline std::filesystem::path directory(HMODULE module=nullptr) {
    wchar_t path[32768]{};
    DWORD n=GetModuleFileNameW(module,path,32768);
    if(!n || n>=32768) return {};
    return std::filesystem::path(std::wstring(path,n)).parent_path();
}
inline std::wstring hr(HRESULT value) {
    std::wostringstream s;
    s<<L"0x"<<std::uppercase<<std::hex<<std::setw(8)<<std::setfill(L'0')
     <<static_cast<unsigned long>(value);
    return s.str();
}
inline bool append(std::filesystem::path const& dir, wchar_t const* name,
                   std::wstring_view line) noexcept {
    try {
        if(dir.empty())return false;
        SYSTEMTIME t{};GetSystemTime(&t);
        std::wostringstream s;
        s<<t.wYear<<L"-"<<t.wMonth<<L"-"<<t.wDay<<L"T"<<t.wHour<<L":"
         <<t.wMinute<<L":"<<t.wSecond<<L"."<<t.wMilliseconds<<L"Z pid="
         <<GetCurrentProcessId()<<L" tid="<<GetCurrentThreadId()<<L" "<<line<<L"\r\n";
        auto str=s.str();
        int n=WideCharToMultiByte(CP_UTF8,0,str.data(),static_cast<int>(str.size()),nullptr,0,nullptr,nullptr);
        if(n<=0)return false;
        std::string utf8(n,'\0');
        if(!WideCharToMultiByte(CP_UTF8,0,str.data(),static_cast<int>(str.size()),utf8.data(),n,nullptr,nullptr))return false;
        auto path=dir/name;
        HANDLE file=CreateFileW(path.c_str(),FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                               nullptr,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(file==INVALID_HANDLE_VALUE){OutputDebugStringW(L"JinJu: diagnostic file open failed\n");return false;}
        DWORD written=0;
        bool ok=WriteFile(file,utf8.data(),static_cast<DWORD>(utf8.size()),&written,nullptr) &&
                written==utf8.size();
        CloseHandle(file);
        return ok;
    } catch(...) { return false; }
}
inline std::wstring clean(std::wstring_view value) {
    std::wstring s(value);
    for(auto& ch:s)if(ch==L'\n'||ch==L'\r'||ch==L'\t')ch=L' ';
    return s;
}
}
