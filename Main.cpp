/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Application entry point. Boots the completed Core systems (Memory,
* Logging, Config) and runs the Engine.

*/

#include "Core/Memory/MemoryManager.h"
#include "Core/Logging/Logger.h"
#include "Core/Config/ConfigManager.h"

using namespace XR3D::Core;

int main(int argc, char** argv)
{
    // Init order: Memory first (nothing depends on it), then Logging
    // (so Config can log its own load result), then Config.
    Memory::MemoryManager::Get().Initialize();
    Logging::Logger::Get().Initialize();
    Config::ConfigManager::Get().Initialize();

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
    // (Time, FileSystem, ...) are implemented; Memory, Logging and Config
    // are wired in directly here since they are the only Core systems
    // complete so far.

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine running.");

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine shutting down...");

    // Shutdown order: reverse of init - Config, then Logging (so it can
    // flush its last messages), then Memory.
    Config::ConfigManager::Get().Shutdown();
    Logging::Logger::Get().Shutdown();
    Memory::MemoryManager::Get().Shutdown();

    return 0;
}