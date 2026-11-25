#pragma once
#include "cleanup_request.h"
#include <winrt/Windows.Foundation.h>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

namespace cleanup {
// One receiver for all subscriptions to this DLL. Its MTA worker can wait for
// individual XAML apartments without blocking any of their message pumps.
class Runtime : public std::enable_shared_from_this<Runtime> {
public:
    using Action=std::function<winrt::Windows::Foundation::IAsyncAction()>;
    static std::shared_ptr<Runtime> subscribe(uintptr_t key, Action disconnect) {
        std::lock_guard lock(mutex);
        auto active=current.lock();
        if (!active || active->retired) {
            active=std::make_shared<Runtime>();
            active->start();
            current=active;
        }
        if (active->closing) throw winrt::hresult_error(HRESULT_FROM_WIN32(ERROR_BUSY));
        active->sites.emplace(key,std::move(disconnect));
        return active;
    }
    bool stopping() const { return closing.load(); }
    bool trackUi(DWORD thread, Action clear) {
        std::lock_guard lock(mutex);
        if (closing) return false;
        dispatchers.try_emplace(thread,std::move(clear));
        return true;
    }
    void forget(uintptr_t key, bool closeWhenEmpty) {
        std::lock_guard lock(mutex);
        sites.erase(key);
        if (sites.empty() && closeWhenEmpty && !closing.exchange(true))
            PostMessageW(window,removeMessage,0,0);
    }
private:
    using Actions=std::map<uintptr_t,Action>;
    inline static std::mutex mutex;
    inline static std::weak_ptr<Runtime> current;
    Actions sites,dispatchers;
    std::atomic_bool closing=false;
    bool retired=false;
    HWND window=nullptr;

    static LRESULT CALLBACK windowProc(HWND hwnd,UINT msg,WPARAM w,LPARAM l) noexcept {
        if (msg==WM_NCCREATE) {
            auto create=reinterpret_cast<CREATESTRUCTW*>(l);
            SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(create->lpCreateParams));
        }
        if (msg==removeMessage && w==0 && l==0) {
            auto runtime=reinterpret_cast<Runtime*>(GetWindowLongPtrW(hwnd,GWLP_USERDATA));
            try { return runtime && runtime->clean() ? 1 : 0; } catch (...) { return 0; }
        }
        // End this worker only after the owned resources have been released.
        if (msg==WM_CLOSE) return 0;
        return DefWindowProcW(hwnd,msg,w,l);
    }
    void start() {
        std::promise<HWND> ready;
        auto opened=ready.get_future();
        std::thread([self=shared_from_this(),ready=std::move(ready)]() mutable {
            HMODULE module{};
            HWND hwnd=nullptr;
            bool initialized=false;
            try {
                winrt::init_apartment(winrt::apartment_type::multi_threaded);
                initialized=true;
                winrt::check_bool(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&windowProc),&module));
                WNDCLASSW wc{};
                wc.hInstance=module; wc.lpfnWndProc=windowProc; wc.lpszClassName=windowClass;
                if (!RegisterClassW(&wc) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS) winrt::throw_last_error();
                hwnd=CreateWindowExW(0,windowClass,L"",0,0,0,0,0,HWND_MESSAGE,nullptr,module,self.get());
                winrt::check_bool(hwnd!=nullptr);
                ready.set_value(hwnd);
                MSG msg{};
                while (GetMessageW(&msg,nullptr,0,0)>0) DispatchMessageW(&msg);
            } catch (...) {
                if (!hwnd) ready.set_exception(std::current_exception());
            }
            if (hwnd) DestroyWindow(hwnd);
            if (module) UnregisterClassW(windowClass,module);
            if (initialized) winrt::uninit_apartment();
        }).detach();
        window=opened.get();
    }
    void drain(Actions& registered) {
        Actions pending;
        { std::lock_guard lock(mutex); pending=registered; }
        for (auto const& [key,close] : pending) {
            try { close().get(); }
            catch (...) { continue; } // Keep this entry for the next request.
            std::lock_guard lock(mutex);
            registered.erase(key);
        }
    }
    bool clean() {
        closing=true;
        drain(sites);
        drain(dispatchers);
        std::lock_guard lock(mutex);
        if (!sites.empty() || !dispatchers.empty()) return false;
        retired=true;
        PostQuitMessage(0);
        return true;
    }
};
}
