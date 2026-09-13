/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares TimeTimer, a single one-shot or repeating timer. Owned and
* driven by TimeRegistry - never used standalone by other modules.

*/

#pragma once

#include "TimeTypes.h"

#include <functional>

namespace XR3D::Core::Time
{
    class TimeTimer
    {
    public:
        void Configure(double durationSeconds, TimerType type, std::function<void()> callback);
        void Reset();

        // Advances elapsed time. Returns how many times the callback is
        // now due to fire this update (0 = not due, 1 = OneShot fired
        // or Repeating fired once, 2+ = a Repeating timer whose delta
        // spanned more than one full interval in a single call - e.g.
        // after a long frame stall). TimeTimer itself NEVER invokes the
        // callback directly; the caller (TimeRegistry) fires it that
        // many times once it is safe to do so (after releasing any
        // locks), to avoid re-entrancy/deadlock if the callback creates
        // or destroys other timers.
        int Advance(double deltaSeconds);

        bool IsActive() const { return m_active; }
        double GetElapsed() const { return m_elapsed; }
        double GetDuration() const { return m_duration; }
        const std::function<void()>& GetCallback() const { return m_callback; }

    private:
        double m_duration = 0.0;
        double m_elapsed = 0.0;
        TimerType m_type = TimerType::OneShot;
        std::function<void()> m_callback;
        bool m_active = false;
    };
}