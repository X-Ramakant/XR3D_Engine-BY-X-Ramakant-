/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares TimeClock: the Engine's main clock. Tracks delta time, total
* elapsed time, time-scale, pause state, and the fixed-timestep
* accumulator used for deterministic Physics-style updates.

*/

#pragma once

#include "TimeTypes.h"

namespace XR3D::Core::Time
{
    class TimeClock
    {
    public:
        void Start();

        // Advances the clock by one frame. Call once per frame from the
        // Engine's main loop.
        void Tick();

        double GetDeltaTime() const { return m_scaledDeltaTime; }
        double GetUnscaledDeltaTime() const { return m_unscaledDeltaTime; }
        double GetTotalTime() const { return m_totalTime; }

        void SetTimeScale(double scale) { m_timeScale = scale; }
        double GetTimeScale() const { return m_timeScale; }

        void Pause() { m_paused = true; }
        void Resume() { m_paused = false; }
        bool IsPaused() const { return m_paused; }

        // Caps how large a single frame's delta time can be. Protects
        // against the "spiral of death" - if a frame takes an unusually
        // long real-world time (breakpoint, disk stall, OS hiccup), the
        // next Tick() would otherwise report a huge delta, causing
        // Physics/Animation to take one giant, unstable step.
        void SetMaxDeltaTime(double maxSeconds) { m_maxDeltaTime = maxSeconds; }
        double GetMaxDeltaTime() const { return m_maxDeltaTime; }

        void SetFixedDeltaTime(double seconds) { m_fixedDeltaTime = seconds; }
        double GetFixedDeltaTime() const { return m_fixedDeltaTime; }

        // Fixed-timestep accumulator, for deterministic Physics-style
        // updates decoupled from variable frame rate. Usage:
        //   while (clock.ConsumeFixedStep()) { PhysicsStep(clock.GetFixedDeltaTime()); }
        bool ConsumeFixedStep();

    private:
        TimePoint m_lastTickTime{};
        bool m_started = false;

        double m_unscaledDeltaTime = 0.0;
        double m_scaledDeltaTime = 0.0;
        double m_totalTime = 0.0;
        double m_timeScale = 1.0;
        bool m_paused = false;

        double m_maxDeltaTime = 0.25; // 250 ms

        double m_fixedDeltaTime = 1.0 / 60.0; // 60 Hz by default
        double m_fixedAccumulator = 0.0;

        // TODO (Future): make maxDeltaTime/fixedDeltaTime configurable
        // via Core/Config once that integration point is wired up here.
    };
}