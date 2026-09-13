/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Defines shared types used across the Time system: the Clock/TimePoint/
* Duration aliases and the TimerType enum.

*/

#pragma once

#include <chrono>
#include <cstdint>

namespace XR3D::Core::Time
{
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<double>; // seconds, double precision

    enum class TimerType : uint8_t
    {
        OneShot = 0,
        Repeating,
        Count
    };

    // TODO (Future): add a TimeUnit enum (Seconds/Milliseconds/Frames)
    // if a formatting/conversion utility is ever needed.
}