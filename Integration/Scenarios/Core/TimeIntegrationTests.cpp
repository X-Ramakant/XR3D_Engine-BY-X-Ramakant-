/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Integration scenarios for Core/Time: verifies realistic engine-like
* usage (a simulated game loop driving fixed-timestep physics, an
* ability-cooldown-style timer system, and a pause-menu scenario)
* stays crash-free and correct once Time is running inside the Engine.

*/

#include "IntegrationTestFramework.h"
#include "Core\Time\TimeManager.h"

#include <chrono>
#include <thread>

using namespace XR3D::Core::Time;

XR3D_SCENARIO(Integration_Time_SimulatedGameLoop_FixedStepDrivesPhysics)
{
    TimeManager::Get().Initialize();
    TimeManager::Get().SetFixedDeltaTime(1.0 / 60.0);

    int totalPhysicsSteps = 0;
    const int kFrameCount = 10;

    for (int frame = 0; frame < kFrameCount; ++frame)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS frame
        TimeManager::Get().Tick();

        while (TimeManager::Get().ConsumeFixedStep())
        {
            ++totalPhysicsSteps; // stand-in for a real PhysicsStep() call
        }
    }

    // Loose bounds intentionally - std::this_thread::sleep_for() is not
    // precise (OS scheduler / timer resolution jitter, especially in
    // Debug builds), so this checks the fixed-step system behaves
    // sanely rather than pinning an exact frame-timing outcome.
    XR3D_CHECK(totalPhysicsSteps >= 5);
    XR3D_CHECK(totalPhysicsSteps <= 20);

    TimeManager::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_Time_AbilityCooldownSystem)
{
    TimeManager::Get().Initialize();

    // Simulate three "ability cooldowns" with different durations,
    // the way a gameplay system would use one-shot timers.
    int fireballReady = 0;
    int healReady = 0;
    int dashReady = 0;

    TimeManager::Get().CreateTimer(0.05, TimerType::OneShot, [&]() { fireballReady++; });
    TimeManager::Get().CreateTimer(0.10, TimerType::OneShot, [&]() { healReady++; });
    TimeManager::Get().CreateTimer(0.02, TimerType::OneShot, [&]() { dashReady++; });

    // Simulate several small frame ticks rather than one huge jump, the
    // way a real cooldown bar would update every frame.
    for (int i = 0; i < 20; ++i)
    {
        TimeManager::Get().GetRegistry().UpdateAll(0.01); // 10ms steps, 200ms total
    }

    XR3D_CHECK_EQ(dashReady, 1);
    XR3D_CHECK_EQ(fireballReady, 1);
    XR3D_CHECK_EQ(healReady, 1);
    XR3D_CHECK_EQ(TimeManager::Get().GetActiveTimerCount(), size_t{ 0 });

    TimeManager::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_Time_PauseMenuScenario)
{
    TimeManager::Get().Initialize();

    // A gameplay timer (e.g. an enemy spawn timer) that must NOT
    // advance while the pause menu is open.
    int spawnCount = 0;
    TimeManager::Get().CreateTimer(0.05, TimerType::Repeating, [&]() { spawnCount++; });

    TimeManager::Get().Tick(); // establish a baseline tick

    TimeManager::Get().Pause();
    for (int i = 0; i < 5; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        TimeManager::Get().Tick();
        TimeManager::Get().GetRegistry().UpdateAll(TimeManager::Get().GetDeltaTime());
    }

    XR3D_CHECK_EQ(spawnCount, 0); // nothing should have fired while paused

    TimeManager::Get().Resume();
    TimeManager::Get().GetRegistry().UpdateAll(0.06); // one interval's worth, now unpaused

    XR3D_CHECK(spawnCount >= 1); // resumes firing normally after Resume()

    TimeManager::Get().Shutdown();
    return true;
}