/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Application entry point. Boots the CoreManager and starts the Engine.

*/

#include "Core/Memory/MemoryManager.h"

#include <cstdio>

int main(int argc, char** argv)
{
    printf("XR3D Engine starting...\n");

    XR3D::Core::Memory::MemoryManager::Get().Initialize();

    // TODO (Future): call CoreManager::Initialize once all Core systems
    // (Logging, Config, Time, FileSystem, ...) are implemented; Memory
    // is currently wired in directly since it is the only Core system
    // complete so far.

    printf("XR3D Engine running.\n");

    XR3D::Core::Memory::MemoryManager::Get().Shutdown();

    printf("XR3D Engine shut down cleanly.\n");

    return 0;
}