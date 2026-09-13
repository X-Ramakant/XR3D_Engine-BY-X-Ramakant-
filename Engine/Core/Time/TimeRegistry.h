/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares TimeRegistry, which owns every active timer in a slot-map
* keyed by generational TimerHandle, and drives them all forward each
* frame.

*/

#pragma once

#include "TimeHandle.h"
#include "TimeTimer.h"
#include "TimeTypes.h"

#include <functional>
#include <mutex>
#include <vector>

namespace XR3D::Core::Time
{
    class TimeRegistry
    {
    public:
        TimerHandle CreateTimer(double durationSeconds, TimerType type, std::function<void()> callback);
        void DestroyTimer(TimerHandle handle);
        bool IsValid(TimerHandle handle) const;

        // Advances every active timer by deltaSeconds. Due callbacks are
        // collected during the locked pass and fired only AFTER the lock
        // is released, so a callback that creates/destroys other timers
        // never deadlocks or corrupts mid-iteration state.
        void UpdateAll(double deltaSeconds);

        size_t GetActiveCount() const;

    private:
        struct Slot
        {
            TimeTimer timer;
            uint32_t generation = 0;
            bool occupied = false;
        };

        mutable std::mutex m_mutex;
        std::vector<Slot> m_slots;
        std::vector<uint32_t> m_freeIndices;

        // TODO (Future): switch to a priority queue / sorted structure
        // if the number of concurrent timers grows large enough that
        // linear UpdateAll() scanning becomes a measured bottleneck.
    };
}