/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements TimeManager.

*/

#include "TimeManager.h"

namespace XR3D::Core::Time
{
    TimeManager& TimeManager::Get()
    {
        static TimeManager instance;
        return instance;
    }

    void TimeManager::Initialize()
    {
        if (m_initialized)
        {
            return;
        }

        m_clock.Start();
        m_initialized = true;
    }

    void TimeManager::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        m_initialized = false;
    }

    void TimeManager::Tick()
    {
        m_clock.Tick();
        m_registry.UpdateAll(m_clock.GetDeltaTime());
    }

    TimerHandle TimeManager::CreateTimer(double durationSeconds, TimerType type, std::function<void()> callback)
    {
        return m_registry.CreateTimer(durationSeconds, type, std::move(callback));
    }

    void TimeManager::DestroyTimer(TimerHandle handle)
    {
        m_registry.DestroyTimer(handle);
    }

    bool TimeManager::IsTimerValid(TimerHandle handle) const
    {
        return m_registry.IsValid(handle);
    }

    size_t TimeManager::GetActiveTimerCount() const
    {
        return m_registry.GetActiveCount();
    }
}