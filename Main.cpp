/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Application entry point. Boots the completed Core systems (Memory,
* Logging, Config, Time) and runs the Engine.

*/

#include "Core/Memory/MemoryManager.h"
#include "Core/Logging/Logger.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Time/TimeManager.h"

using namespace XR3D::Core;

int main(int argc, char** argv)
{
    // Init order: Memory first, then Logging (so Config/Time can log
    // their own results), then Config, then Time.
    Memory::MemoryManager::Get().Initialize();
    Logging::Logger::Get().Initialize();
    Config::ConfigManager::Get().Initialize();
    Time::TimeManager::Get().Initialize();

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine starting...");

    if (!Config::ConfigManager::Get().LoadFromFile("engine.cfg"))
    {
        XR3D_LOG_WARNING(Logging::LogCategory::Core,
            "engine.cfg not found - continuing with default settings.");
    }

    const int64_t windowWidth = Config::ConfigManager::Get().GetOrDefault<int64_t>("window.width", 1280);
    const int64_t windowHeight = Config::ConfigManager::Get().GetOrDefault<int64_t>("window.height", 720);

    XR3D_LOG_INFO(Logging::LogCategory::Core, "Resolved window size: {}x{}", windowWidth, windowHeight);

    // TODO (Future): call CoreManager::Initialize once all Core systems
    // (FileSystem, Threading, ...) are implemented; Memory, Logging,
    // Config and Time are wired in directly here since they are the
    // only Core systems complete so far.

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine running.");

    // Demonstrate one frame tick, the way the real main loop will call
    // this every frame once Window/Rendering exist.
    Time::TimeManager::Get().Tick();
    XR3D_LOG_INFO(Logging::LogCategory::Core, "First frame delta time: {} s", Time::TimeManager::Get().GetDeltaTime());

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine shutting down...");

    // Shutdown order: reverse of init - Time, Config, Logging, Memory.
    Time::TimeManager::Get().Shutdown();
    Config::ConfigManager::Get().Shutdown();
    Logging::Logger::Get().Shutdown();
    Memory::MemoryManager::Get().Shutdown();

    return 0;
}