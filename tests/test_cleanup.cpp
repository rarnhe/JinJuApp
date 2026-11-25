#include "../xaml_experiment/cleanup_runtime.h"
#include <atomic>
#include <iostream>

using winrt::Windows::Foundation::IAsyncAction;
IAsyncAction countClose(std::atomic_uint& calls,bool failFirst) {
    if (++calls==1 && failFirst) throw winrt::hresult_error(E_FAIL);
    co_return;
}
int main() {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    std::atomic_uint siteCalls{},uiCalls{},secondSite{},unused{};
    const auto pid=GetCurrentProcessId();
    try {
        if (cleanup::request(pid)!=cleanup::Result::absent) return 1;
        auto first=cleanup::Runtime::subscribe(1,[&] { return countClose(siteCalls,true); });
        auto second=cleanup::Runtime::subscribe(2,[&] { return countClose(secondSite,false); });
        if (first!=second || !first->trackUi(10,[&] { return countClose(uiCalls,false); }) ||
            !first->trackUi(10,[&] { return countClose(unused,false); })) return 2;
        if (cleanup::request(pid)!=cleanup::Result::failed || !first->stopping() ||
            first->trackUi(11,[&] { return countClose(unused,false); })) return 3;
        if (cleanup::request(pid)!=cleanup::Result::removed ||
            siteCalls!=2 || uiCalls!=1 || secondSite!=1 || unused!=0) return 4;

        // The next explicit activation gets independent state after removal.
        auto next=cleanup::Runtime::subscribe(3,[&] { return countClose(unused,false); });
        if (first==next || next->stopping() || cleanup::request(pid)!=cleanup::Result::removed ||
            unused!=1) return 5;
        auto again=cleanup::request(pid);
        if (again==cleanup::Result::failed || unused!=1) return 6;
    } catch (winrt::hresult_error const& e) {
        std::wcerr<<L"Cleanup HRESULT "<<std::hex<<e.code().value<<L" "<<e.message().c_str()<<L"\n";
        return 7;
    }
}
