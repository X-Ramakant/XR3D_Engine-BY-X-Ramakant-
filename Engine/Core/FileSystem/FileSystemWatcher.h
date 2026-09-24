/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares FileSystemWatcher, a polling-based file change watcher using
* generational WatchHandles for safe create/destroy.

*/

#pragma once

#include "FileSystemTypes.h"

#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace XR3D::Core::FileSystem
{
    // Polling-based file change watcher. Call PollAll() periodically
    // (e.g. once per frame, or on its own timer) to check for changes.
    //
    // TODO (Future): replace with OS-native events (e.g. Windows
    // ReadDirectoryChangesW) via Platform/Windows once that exists,
    // instead of polling last-write-time - polling has a detection
    // delay of up to one PollAll() interval and does not scale to very
    // large watch counts.

    class FileSystemWatcher
    {
    public:
        WatchHandle Watch(const std::string& path, std::function<void()> onChanged);
        void Unwatch(WatchHandle handle);
        bool IsValid(WatchHandle handle) const;

        // Checks every watched path's last-write-time; fires onChanged
        // for any that changed since the previous poll. Due callbacks
        // are collected under the lock and fired only AFTER releasing
        // it - same reentrancy-safety pattern as Core/Time's
        // TimeRegistry::UpdateAll(), so a callback that calls
        // Watch()/Unwatch() again can never deadlock.
        void PollAll();

        size_t GetWatchCount() const;

    private:
        struct Slot
        {
            std::string path;
            std::filesystem::file_time_type lastWriteTime{};
            std::function<void()> onChanged;
            uint32_t generation = 0;
            bool occupied = false;
        };

        mutable std::mutex m_mutex;
        std::vector<Slot> m_slots;
        std::vector<uint32_t> m_freeIndices;
    };
}