/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements TimeClock.

*/

#include "TimeClock.h"

#include <algorithm>

namespace XR3D::Core::Time
{
    void TimeClock::Start()
    {
        m_lastTickTime = Clock::now();
        m_started = true;
        m_totalTime = 0.0;
        m_fixedAccumulator = 0.0;
        m_unscaledDeltaTime = 0.0;
        m_scaledDeltaTime = 0.0;
    }

    void TimeClock::Tick()
    {
        if (!m_started)
        {
            Start();
            return;
        }

        const TimePoint now = Clock::now();
        double rawDelta = std::chrono::duration_cast<Duration>(now - m_lastTickTime).count();
        m_lastTickTime = now;

        rawDelta = std::min(rawDelta, m_maxDeltaTime);
        m_unscaledDeltaTime = rawDelta;

        if (m_paused)
        {
            m_scaledDeltaTime = 0.0;
        }
        else
        {
            m_scaledDeltaTime = rawDelta * m_timeScale;
            m_totalTime += m_scaledDeltaTime;
            m_fixedAccumulator += m_scaledDeltaTime;
        }
    }

    bool TimeClock::ConsumeFixedStep()
    {
        if (m_fixedAccumulator >= m_fixedDeltaTime)
        {
            m_fixedAccumulator -= m_fixedDeltaTime;
            return true;
        }
        return false;
    }
}