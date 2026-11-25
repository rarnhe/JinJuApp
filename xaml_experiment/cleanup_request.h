#pragma once
#include <windows.h>
#include <iostream>
#include <vector>

namespace cleanup {
inline constexpr wchar_t windowClass[]=L"JinJu.Weather.Cleanup.v1";
inline constexpr UINT removeMessage=WM_APP+49;
enum class Result { absent, removed, failed };

inline Result request(DWORD ownerPid) {
    std::vector<HWND> receivers;
    HWND after=nullptr;
    while ((after=FindWindowExW(HWND_MESSAGE,after,windowClass,nullptr))!=nullptr) {
        DWORD pid{};
        GetWindowThreadProcessId(after,&pid);
        if (pid==ownerPid) receivers.push_back(after);
    }
    for (auto receiver : receivers) {
        DWORD_PTR acknowledged=0;
        SetLastError(ERROR_SUCCESS);
        auto sent=SendMessageTimeoutW(receiver,removeMessage,0,0,SMTO_BLOCK|SMTO_ABORTIFHUNG,
                                      15000,&acknowledged);
        if (!sent || acknowledged!=1) {
            DWORD pid{};
            if (!GetWindowThreadProcessId(receiver,&pid) || pid!=ownerPid) continue;
            return Result::failed;
        }
    }
    return receivers.empty() ? Result::absent : Result::removed;
}
inline int removeCurrentGreeting() {
    DWORD pid{};
    if (!GetWindowThreadProcessId(FindWindowW(L"Shell_TrayWnd",nullptr),&pid) || !pid) {
        std::wcerr<<L"Could not identify the taskbar in this session.\n";
        return 2;
    }
    switch (request(pid)) {
    case Result::absent: std::wcout<<L"No active JinJu cleanup endpoint in this taskbar session.\n"; return 0;
    case Result::removed: std::wcout<<L"Greeting removed and XAML observers disconnected.\n"; return 0;
    default: std::wcerr<<L"Cleanup did not complete. Run --remove again to retry.\n"; return 1;
    }
}
}
