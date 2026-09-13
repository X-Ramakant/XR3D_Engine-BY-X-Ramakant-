/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements TimeRegistry.

*/

#include "TimeRegistry.h"

namespace XR3D::Core::Time
{
    TimerHandle TimeRegistry::CreateTimer(double durationSeconds, TimerType type, std::function<void()> callback)
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
        slot.timer.Configure(durationSeconds, type, std::move(callback));
        slot.occupied = true;

        TimerHandle handle;
        handle.index = index;
        handle.generation = slot.generation;
        return handle;
    }

    void TimeRegistry::DestroyTimer(TimerHandle handle)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (handle.index >= m_slots.size())
        {
            return;
        }

        Slot& slot = m_slots[handle.index];
        if (!slot.occupied || slot.generation != handle.generation)
        {
            return; // Stale handle - already destroyed/reused, ignore.
        }

        slot.occupied = false;
        slot.generation += 1;
        m_freeIndices.push_back(handle.index);
    }

    bool TimeRegistry::IsValid(TimerHandle handle) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (handle.index >= m_slots.size())
        {
            return false;
        }

        const Slot& slot = m_slots[handle.index];
        return slot.occupied && slot.generation == handle.generation;
    }

    void TimeRegistry::UpdateAll(double deltaSeconds)
    {
        std::vector<std::function<void()>> dueCallbacks;

        {
            std::lock_guard<std::mutex> lock(m_mutex);

            for (size_t i = 0; i < m_slots.size(); ++i)
            {
                Slot& slot = m_slots[i];
                if (!slot.occupied)
                {
                    continue;
                }

                const int dueCount = slot.timer.Advance(deltaSeconds);
                if (dueCount > 0 && slot.timer.GetCallback())
                {
                    for (int n = 0; n < dueCount; ++n)
                    {
                        dueCallbacks.push_back(slot.timer.GetCallback());
                    }
                }

                if (!slot.timer.IsActive())
                {
                    slot.occupied = false;
                    slot.generation += 1;
                    m_freeIndices.push_back(static_cast<uint32_t>(i));
                }
            }
        } // lock released here, before any callback fires

        for (auto& callback : dueCallbacks)
        {
            callback();
        }
    }

    size_t TimeRegistry::GetActiveCount() const
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