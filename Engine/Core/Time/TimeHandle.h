/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares TimerHandle, a generational handle that safely references a
* timer slot in TimeRegistry without risking dangling references.

*/

#pragma once

#include <cstdint>

namespace XR3D::Core::Time
{
    // A generational handle: index alone is not safe to keep around,
    // because a destroyed timer's slot gets reused by a later timer.
    // The generation counter lets IsValid()/DestroyTimer() detect a
    // stale handle that still points at index N, but N has since been
    // recycled for an unrelated timer.
    struct TimerHandle
    {
        static constexpr uint32_t kInvalidIndex = 0xFFFFFFFFu;

        uint32_t index = kInvalidIndex;
        uint32_t generation = 0;

        bool IsValid() const { return index != kInvalidIndex; }

        bool operator==(const TimerHandle& other) const
        {
            return index == other.index && generation == other.generation;
        }

        bool operator!=(const TimerHandle& other) const
        {
            return !(*this == other);
        }
    };

    inline constexpr TimerHandle kInvalidTimerHandle{};
}