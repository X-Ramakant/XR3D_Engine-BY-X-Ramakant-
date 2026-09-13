#include "TestFramework.h"
#include "Core\Time\TimeManager.h"

#include <chrono>
#include <thread>

using namespace XR3D::Core::Time;

XR3D_TEST(Time_Manager_InitializeAndShutdown)
{
    TimeManager::Get().Initialize();
    TimeManager::Get().Initialize(); // second call must be a safe no-op
    TimeManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Time_Clock_DeltaTimeIsPositiveAfterTick)
{
    TimeManager::Get().Initialize();

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    TimeManager::Get().Tick();

    XR3D_CHECK(TimeManager::Get().GetDeltaTime() > 0.0);
    XR3D_CHECK(TimeManager::Get().GetDeltaTime() < 1.0);

    TimeManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Time_Clock_PauseSetsZeroDelta)
{
    TimeManager::Get().Initialize();

    TimeManager::Get().Pause();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    TimeManager::Get().Tick();

    XR3D_CHECK_EQ(TimeManager::Get().GetDeltaTime(), 0.0);
    XR3D_CHECK(TimeManager::Get().IsPaused());

    TimeManager::Get().Resume();
    XR3D_CHECK(!TimeManager::Get().IsPaused());

    TimeManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Time_Clock_TimeScaleAffectsDelta)
{
    TimeManager::Get().Initialize();
    TimeManager::Get().SetTimeScale(2.0);

    XR3D_CHECK_EQ(TimeManager::Get().GetTimeScale(), 2.0);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    TimeManager::Get().Tick();

    // Scaled delta should be roughly double the unscaled delta.
    double scaled = TimeManager::Get().GetDeltaTime();
    double unscaled = TimeManager::Get().GetUnscaledDeltaTime();
    XR3D_CHECK(scaled > unscaled * 1.5);

    TimeManager::Get().SetTimeScale(1.0);
    TimeManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Time_Clock_MaxDeltaTimeClamps)
{
    TimeManager::Get().Initialize();
    TimeManager::Get().SetMaxDeltaTime(0.03);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    TimeManager::Get().Tick();

    XR3D_CHECK(TimeManager::Get().GetUnscaledDeltaTime() <= 0.03 + 0.001);

    TimeManager::Get().SetMaxDeltaTime(0.25); // restore default - singleton state must not leak into later tests
    TimeManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Time_Timer_OneShot_FiresOnceAndCleansUp)
{
    TimeManager::Get().Initialize();

    int fireCount = 0;
    TimerHandle handle = TimeManager::Get().CreateTimer(0.01, TimerType::OneShot, [&]() { fireCount++; });
    XR3D_CHECK(TimeManager::Get().IsTimerValid(handle));

    TimeManager::Get().GetRegistry().UpdateAll(0.02);

    XR3D_CHECK_EQ(fireCount, 1);
    XR3D_CHECK(!TimeManager::Get().IsTimerValid(handle)); // slot recycled after firing

    TimeManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Time_Timer_Repeating_FiresMultipleTimesForLargeDelta)
{
    TimeManager::Get().Initialize();

    int fireCount = 0;
    TimerHandle handle = TimeManager::Get().CreateTimer(0.1, TimerType::Repeating, [&]() { fireCount++; });

    TimeManager::Get().GetRegistry().UpdateAll(0.35); // spans 3 full intervals

    XR3D_CHECK_EQ(fireCount, 3);
    XR3D_CHECK(TimeManager::Get().IsTimerValid(handle)); // repeating timer stays alive

    TimeManager::Get().DestroyTimer(handle);
    TimeManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Time_Timer_StaleHandle_DetectedAfterReuse)
{
    TimeManager::Get().Initialize();

    TimerHandle first = TimeManager::Get().CreateTimer(1.0, TimerType::OneShot, []() {});
    TimeManager::Get().DestroyTimer(first);
    XR3D_CHECK(!TimeManager::Get().IsTimerValid(first));

    TimerHandle second = TimeManager::Get().CreateTimer(1.0, TimerType::OneShot, []() {});

    // The slot index is very likely reused - the important guarantee is
    // that the OLD handle is never mistaken for the NEW one.
    XR3D_CHECK(!TimeManager::Get().IsTimerValid(first));
    XR3D_CHECK(TimeManager::Get().IsTimerValid(second));
    XR3D_CHECK(first != second);

    TimeManager::Get().DestroyTimer(second);
    TimeManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Time_FixedTimestep_ConsumeFixedStepWorks)
{
    TimeManager::Get().Initialize();
    TimeManager::Get().SetMaxDeltaTime(0.25); // defensive: don't depend on other tests' settings
    TimeManager::Get().SetFixedDeltaTime(0.02);

    std::this_thread::sleep_for(std::chrono::milliseconds(70));
    TimeManager::Get().Tick();

    int steps = 0;
    while (TimeManager::Get().ConsumeFixedStep())
    {
        steps++;
    }

    // ~70ms of accumulated time / 20ms fixed step should yield ~3 steps.
    XR3D_CHECK(steps >= 2);

    TimeManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Time_Timer_ReentrantCallback_DoesNotDeadlock)
{
    TimeManager::Get().Initialize();

    bool innerTimerValid = false;
    TimeManager::Get().CreateTimer(0.01, TimerType::OneShot, [&]() {
        TimerHandle inner = TimeManager::Get().CreateTimer(1.0, TimerType::OneShot, []() {});
        innerTimerValid = TimeManager::Get().IsTimerValid(inner);
        });

    // If this call hangs, the test binary itself will hang - reaching
    // the check below at all proves no deadlock occurred.
    TimeManager::Get().GetRegistry().UpdateAll(0.02);

    XR3D_CHECK(innerTimerValid);

    TimeManager::Get().Shutdown();
    return true;
}