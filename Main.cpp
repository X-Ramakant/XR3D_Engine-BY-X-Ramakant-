/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Application entry point. Boots the completed Core systems (Memory,
* Logging, Config, Time, FileSystem) and runs the Engine.

*/

#include "Core/Memory/MemoryManager.h"
#include "Core/Logging/Logger.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Time/TimeManager.h"
#include "Core/FileSystem/FileSystemManager.h"

using namespace XR3D::Core;

int main(int argc, char** argv)
{
    // Init order: Memory first, then Logging, then Config, Time,
    // FileSystem. FileSystem goes last among these since it has no
    // dependents among Memory/Logging/Config/Time yet.
    Memory::MemoryManager::Get().Initialize();
    Logging::Logger::Get().Initialize();
    Config::ConfigManager::Get().Initialize();
    Time::TimeManager::Get().Initialize();
    FileSystem::FileSystemManager::Get().Initialize();

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine starting...");

    if (!Config::ConfigManager::Get().LoadFromFile("engine.cfg"))
    {
        XR3D_LOG_WARNING(Logging::LogCategory::Core,
            "engine.cfg not found - continuing with default settings.");
    }

    const int64_t windowWidth = Config::ConfigManager::Get().GetOrDefault<int64_t>("window.width", 1280);
    const int64_t windowHeight = Config::ConfigManager::Get().GetOrDefault<int64_t>("window.height", 720);

    XR3D_LOG_INFO(Logging::LogCategory::Core, "Resolved window size: {}x{}", windowWidth, windowHeight);

    // Demonstrate FileSystem: check for (and report on) an Assets folder.
    if (FileSystem::FileSystemManager::Get().Exists("Assets"))
    {
        FileSystem::FileSystemManager::Get().RegisterMount("assets", "Assets");
        XR3D_LOG_INFO(Logging::LogCategory::Core, "Assets folder found, mounted as assets://");
    }
    else
    {
        XR3D_LOG_WARNING(Logging::LogCategory::Core, "Assets folder not found - skipping mount.");
    }

    // TODO (Future): call CoreManager::Initialize once all Core systems
    // (Threading, JobSystem, ...) are implemented; Memory, Logging,
    // Config, Time and FileSystem are wired in directly here since they
    // are the only Core systems complete so far.

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine running.");

    // Demonstrate one frame tick, the way the real main loop will call
    // this every frame once Window/Rendering exist.
    Time::TimeManager::Get().Tick();
    XR3D_LOG_INFO(Logging::LogCategory::Core, "First frame delta time: {} s", Time::TimeManager::Get().GetDeltaTime());

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine shutting down...");

    // Shutdown order: reverse of init - FileSystem, Time, Config,
    // Logging, Memory.
    FileSystem::FileSystemManager::Get().Shutdown();
    Time::TimeManager::Get().Shutdown();
    Config::ConfigManager::Get().Shutdown();
    Logging::Logger::Get().Shutdown();
    Memory::MemoryManager::Get().Shutdown();

    return 0;
}