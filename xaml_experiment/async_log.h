#pragma once
#include "diagnostics.h"
#include <algorithm>
#include <cstdint>
#include <deque>
#include <mutex>
#include <utility>
#include <vector>

namespace diagnostics {
enum class LogFile { events, tree };
struct LogEntry {
    LogFile file;
    SYSTEMTIME time;
    DWORD thread;
    std::wstring text;
};

class LogQueue {
public:
    static constexpr size_t capacity = 4096;
    static constexpr size_t batchSize = 256;

    void push(LogEntry entry) {
        if (entries.size() == capacity) {
            ++dropped;
        } else {
            entries.push_back(std::move(entry));
        }
    }
    std::vector<LogEntry> take(uint64_t& lost) {
        std::vector<LogEntry> batch;
        const auto count = (std::min)(entries.size(), batchSize);
        batch.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            batch.push_back(std::move(entries.front()));
            entries.pop_front();
        }
        lost = std::exchange(dropped, 0);
        return batch;
    }
    bool empty() const { return entries.empty(); }
private:
    std::deque<LogEntry> entries;
    uint64_t dropped = 0;
};

class AsyncLog {
public:
    void push(LogFile file, std::wstring_view message) noexcept {
        try {
            SYSTEMTIME time{};
            GetSystemTime(&time);
            LogEntry entry{file, time, GetCurrentThreadId(), std::wstring(message)};
            std::lock_guard lock(mutex);
            queue.push(std::move(entry));
            if (!scheduled) schedule();
        } catch (...) {}
    }
private:
    std::mutex mutex;
    LogQueue queue;
    bool scheduled = false;
    HMODULE module = nullptr;

    // Called under the mutex. A one-shot timer owns one module reference until
    // the callback has returned, including in a host that releases its COM objects.
    void schedule() {
        HMODULE hold{};
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                                reinterpret_cast<LPCWSTR>(&flush), &hold)) return;
        auto timer = CreateThreadpoolTimer(flush, this, nullptr);
        if (!timer) { FreeLibrary(hold); return; }
        module = hold;
        scheduled = true;
        LARGE_INTEGER due{};
        due.QuadPart = -500LL * 10000;
        FILETIME when{due.LowPart, static_cast<DWORD>(due.HighPart)};
        SetThreadpoolTimer(timer, &when, 0, 0);
    }
    static void CALLBACK flush(PTP_CALLBACK_INSTANCE instance, void* context, PTP_TIMER timer) noexcept {
        auto& self = *static_cast<AsyncLog*>(context);
        HMODULE hold;
        { std::lock_guard lock(self.mutex); hold = self.module; }
        FreeLibraryWhenCallbackReturns(instance, hold);
        CloseThreadpoolTimer(timer);
        try {
            const auto dir = directory(hold);
            for (;;) {
                uint64_t dropped;
                std::vector<LogEntry> batch;
                {
                    std::lock_guard lock(self.mutex);
                    if (self.queue.empty()) { self.scheduled = false; return; }
                    batch = self.queue.take(dropped);
                }
                std::wostringstream events, tree;
                for (auto const& line : batch)
                    formatLine(line.file == LogFile::events ? events : tree, line.time, line.thread, line.text);
                if (dropped) {
                    SYSTEMTIME now{};
                    GetSystemTime(&now);
                    formatLine(events, now, GetCurrentThreadId(), L"Log queue dropped=" + std::to_wstring(dropped));
                }
                if (events.tellp() > 0) appendText(dir, L"JinJuXaml.log", events.str());
                if (tree.tellp() > 0) appendText(dir, L"XamlTree.txt", tree.str());
            }
        } catch (...) {
            std::lock_guard lock(self.mutex);
            self.scheduled = false;
        }
    }
};
}
