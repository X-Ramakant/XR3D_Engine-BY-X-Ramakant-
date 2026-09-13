/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements TimeTimer.

*/

#include "TimeTimer.h"

namespace XR3D::Core::Time
{
    void TimeTimer::Configure(double durationSeconds, TimerType type, std::function<void()> callback)
    {
        m_duration = durationSeconds;
        m_type = type;
        m_callback = std::move(callback);
        m_elapsed = 0.0;
        m_active = true;
    }

    void TimeTimer::Reset()
    {
        m_elapsed = 0.0;
        m_active = true;
    }

    int TimeTimer::Advance(double deltaSeconds)
    {
        if (!m_active)
        {
            return 0;
        }

        m_elapsed += deltaSeconds;

        if (m_elapsed < m_duration)
        {
            return 0;
        }

        if (m_type == TimerType::OneShot)
        {
            m_active = false;
            return 1;
        }

        // Repeating: a single large delta (e.g. after a stalled frame)
        // may span more than one full interval - fire once per interval
        // crossed, not just once, so no "beats" are silently dropped.
        int fireCount = 0;
        while (m_elapsed >= m_duration)
        {
            m_elapsed -= m_duration;
            ++fireCount;
        }
        return fireCount;
    }
}