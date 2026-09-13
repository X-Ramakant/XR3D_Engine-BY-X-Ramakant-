/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares TimeManager, the single entry point for all engine timing:
* the frame clock, delta time, time-scale, pause state, fixed-timestep
* accumulator, and timer creation.

*/

#pragma once

#include "TimeClock.h"
#include "TimeHandle.h"
#include "TimeRegistry.h"
#include "TimeTypes.h"

#include <functional>

namespace XR3D::Core::Time
{
    class TimeManager
    {
    public:
        static TimeManager& Get();

        void Initialize();
        void Shutdown();

        // Call once per frame from the Engine's main loop.
        void Tick();

        double GetDeltaTime() const { return m_clock.GetDeltaTime(); }
        double GetUnscaledDeltaTime() const { return m_clock.GetUnscaledDeltaTime(); }
        double GetTotalTime() const { return m_clock.GetTotalTime(); }
        double GetFixedDeltaTime() const { return m_clock.GetFixedDeltaTime(); }

        void SetTimeScale(double scale) { m_clock.SetTimeScale(scale); }
        double GetTimeScale() const { return m_clock.GetTimeScale(); }

        void Pause() { m_clock.Pause(); }
        void Resume() { m_clock.Resume(); }
        bool IsPaused() const { return m_clock.IsPaused(); }

        void SetMaxDeltaTime(double maxSeconds) { m_clock.SetMaxDeltaTime(maxSeconds); }
        void SetFixedDeltaTime(double seconds) { m_clock.SetFixedDeltaTime(seconds); }

        // Usage: while (TimeManager::Get().ConsumeFixedStep()) { PhysicsStep(GetFixedDeltaTime()); }
        bool ConsumeFixedStep() { return m_clock.ConsumeFixedStep(); }

        TimerHandle CreateTimer(double durationSeconds, TimerType type, std::function<void()> callback);
        void DestroyTimer(TimerHandle handle);
        bool IsTimerValid(TimerHandle handle) const;
        size_t GetActiveTimerCount() const;

        TimeClock& GetClock() { return m_clock; }
        TimeRegistry& GetRegistry() { return m_registry; }

    private:
        TimeManager() = default;
        ~TimeManager() = default;
        TimeManager(const TimeManager&) = delete;
        TimeManager& operator=(const TimeManager&) = delete;

        bool m_initialized = false;
        TimeClock m_clock;
        TimeRegistry m_registry;
    };
}