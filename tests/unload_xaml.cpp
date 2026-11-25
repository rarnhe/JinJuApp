#include <windows.h>
#include <restartmanager.h>
#include <iostream>
#include <string_view>
#include <string>
#include <tlhelp32.h>
#include <cwchar>

static bool IsExplorer(DWORD pid, RM_UNIQUE_PROCESS& target)
{
    DWORD ownSession=0, session=0;
    if(!ProcessIdToSessionId(GetCurrentProcessId(),&ownSession) ||
       !ProcessIdToSessionId(pid,&session) || ownSession!=session) return false;
    HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    if(!process)return false;
    wchar_t path[32768]{};
    DWORD length=32768;
    FILETIME exit{},kernel{},user{};
    bool ok=QueryFullProcessImageNameW(process,0,path,&length) &&
            GetProcessTimes(process,&target.ProcessStartTime,&exit,&kernel,&user);
    CloseHandle(process);
    wchar_t windows[MAX_PATH]{};
    if(!GetWindowsDirectoryW(windows,MAX_PATH))return false;
    std::wstring expected=std::wstring(windows)+L"\\explorer.exe";
    if(!ok || _wcsicmp(path,expected.c_str())!=0)return false;
    target.dwProcessId=pid;
    return true;
}

int wmain(int argc,wchar_t** argv)
{
    if(argc==2 && std::wstring_view(argv[1])==L"--self-test"){
        DWORD session=0; wchar_t key[CCH_RM_SESSION_KEY+1]{};
        DWORD result=RmStartSession(&session,0,key);
        if(result!=ERROR_SUCCESS)return 1;
        return RmEndSession(session)==ERROR_SUCCESS?0:2;
    }
    auto tray=FindWindowW(L"Shell_TrayWnd",nullptr);
    DWORD pid=0;
    RM_UNIQUE_PROCESS target{};
    if(!tray || !GetWindowThreadProcessId(tray,&pid) || !IsExplorer(pid,target)){
        std::wcerr<<L"Cannot identify the current session's Explorer. No changes made.\n";return 3;
    }
    if(MessageBoxW(nullptr,L"Restart Windows Explorer to remove JinJuXaml.dll and its XAML changes? Finish any file operations first. The taskbar and Explorer windows may close temporarily.",
                   L"JinJu XAML cleanup",MB_OKCANCEL|MB_ICONWARNING|MB_DEFBUTTON2)!=IDOK)return 0;
    DWORD session=0; wchar_t key[CCH_RM_SESSION_KEY+1]{};
    DWORD result=RmStartSession(&session,0,key);
    if(result!=ERROR_SUCCESS){std::wcerr<<L"RmStartSession: "<<result<<L"\n";return 4;}
    result=RmRegisterResources(session,0,nullptr,1,&target,0,nullptr);
    if(result==ERROR_SUCCESS){
        // Do not force termination; Restart Manager may refuse to restart the shell.
        result=RmShutdown(session,0,nullptr);
        if(result==ERROR_SUCCESS){
            DWORD restart=RmRestart(session,0,nullptr);
            if(restart!=ERROR_SUCCESS)result=restart;
        }
    }
    RmEndSession(session);
    if(result!=ERROR_SUCCESS){
        std::wcerr<<L"Restart Manager returned "<<result<<L". Use Task Manager > Windows Explorer > Restart.\n";
        MessageBoxW(nullptr,L"Automatic graceful restart did not complete. Use Task Manager > Windows Explorer > Restart. No forced process termination was attempted.",L"JinJu XAML cleanup",MB_OK|MB_ICONWARNING);
        return 5;
    }
    // A successful API return is not proof the original process has exited.
    HANDLE old=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    bool exited=false;
    if(old){
        FILETIME creation{},exit{},kernel{},user{};
        if(GetProcessTimes(old,&creation,&exit,&kernel,&user)){
            exited=CompareFileTime(&creation,&target.ProcessStartTime)!=0 ||
                   WaitForSingleObject(old,10000)==WAIT_OBJECT_0;
        }
        CloseHandle(old);
    }else if(GetLastError()==ERROR_INVALID_PARAMETER)exited=true;
    if(!exited){
        MessageBoxW(nullptr,L"Original Explorer exit could not be verified. Restart Windows Explorer manually in Task Manager.",L"JinJu XAML cleanup",MB_OK|MB_ICONWARNING);
        return 6;
    }
    for(int i=0;i<100;++i){
        DWORD newPid=0;
        HWND newTray=FindWindowW(L"Shell_TrayWnd",nullptr);
        RM_UNIQUE_PROCESS current{};
        if(newTray && GetWindowThreadProcessId(newTray,&newPid) && IsExplorer(newPid,current) &&
           (newPid!=pid || CompareFileTime(&current.ProcessStartTime,&target.ProcessStartTime)!=0)){
            std::wcout<<L"Explorer restarted; original process and pinned DLL are gone.\n";return 0;
        }
        Sleep(100);
    }
    MessageBoxW(nullptr,L"Original Explorer exited, but the new taskbar was not verified. Start explorer.exe from Task Manager if necessary.",L"JinJu XAML cleanup",MB_OK|MB_ICONWARNING);
    return 7;
}
