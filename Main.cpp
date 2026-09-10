/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Application entry point. Boots the completed Core systems (Memory,
* Logging) and runs the Engine.

*/

#include "Core/Memory/MemoryManager.h"
#include "Core/Logging/Logger.h"

using namespace XR3D::Core;

int main(int argc, char** argv)
{
    // Init order: Memory first (nothing else depends on it), then Logging.
    Memory::MemoryManager::Get().Initialize();
    Logging::Logger::Get().Initialize();

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine starting...");

    // TODO (Future): call CoreManager::Initialize once all Core systems
    // (Config, Time, FileSystem, ...) are implemented; Memory and Logging
    // are wired in directly here since they are the only Core systems
    // complete so far.

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine running.");

    XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine shutting down...");

    // Shutdown order: reverse of init - Logging first (so it can flush
    // its last messages), then Memory.
    Logging::Logger::Get().Shutdown();
    Memory::MemoryManager::Get().Shutdown();

    return 0;
}