/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements FileSystemWatcher.

*/

#include "FileSystemWatcher.h"

namespace fs = std::filesystem;

namespace XR3D::Core::FileSystem
{
    WatchHandle FileSystemWatcher::Watch(const std::string& path, std::function<void()> onChanged)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        uint32_t index;
        if (!m_freeIndices.empty())
        {
            index = m_freeIndices.back();
            m_freeIndices.pop_back();
        }
        else
        {
            index = static_cast<uint32_t>(m_slots.size());
            m_slots.emplace_back();
        }

        Slot& slot = m_slots[index];
        slot.path = path;
        slot.onChanged = std::move(onChanged);
        slot.occupied = true;

        std::error_code ec;
        slot.lastWriteTime = fs::last_write_time(path, ec);
        // If the path does not exist yet, ec is set and lastWriteTime
        // stays default-constructed; PollAll() will still detect the
        // file's eventual creation as a change once it appears.

        WatchHandle handle;
        handle.index = index;
        handle.generation = slot.generation;
        return handle;
    }

    void FileSystemWatcher::Unwatch(WatchHandle handle)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (handle.index >= m_slots.size())
        {
            return;
        }

        Slot& slot = m_slots[handle.index];
        if (!slot.occupied || slot.generation != handle.generation)
        {
            return; // Stale handle - already removed/reused, ignore.
        }

        slot.occupied = false;
        slot.onChanged = nullptr;
        slot.generation += 1;
        m_freeIndices.push_back(handle.index);
    }

    bool FileSystemWatcher::IsValid(WatchHandle handle) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (handle.index >= m_slots.size())
        {
            return false;
        }

        const Slot& slot = m_slots[handle.index];
        return slot.occupied && slot.generation == handle.generation;
    }

    void FileSystemWatcher::PollAll()
    {
        std::vector<std::function<void()>> dueCallbacks;

        {
            std::lock_guard<std::mutex> lock(m_mutex);

            for (Slot& slot : m_slots)
            {
                if (!slot.occupied)
                {
                    continue;
                }

                std::error_code ec;
                const fs::file_time_type currentWriteTime = fs::last_write_time(slot.path, ec);

                if (!ec && currentWriteTime != slot.lastWriteTime)
                {
                    slot.lastWriteTime = currentWriteTime;
                    if (slot.onChanged)
                    {
                        dueCallbacks.push_back(slot.onChanged);
                    }
                }
            }
        } // lock released here, before any callback fires

        for (auto& callback : dueCallbacks)
        {
            callback();
        }
    }

    size_t FileSystemWatcher::GetWatchCount() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        size_t count = 0;
        for (const Slot& slot : m_slots)
        {
            if (slot.occupied)
            {
                ++count;
            }
        }
        return count;
    }
}