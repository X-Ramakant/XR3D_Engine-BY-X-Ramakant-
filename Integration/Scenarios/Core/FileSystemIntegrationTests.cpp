/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Integration scenarios for Core/FileSystem: verifies realistic
* engine-like usage (mounted asset loading at boot, config hot-reload
* via the watcher, and first-run save-directory creation) stays
* crash-free once FileSystem is running inside the Engine.

*/

#include "IntegrationTestFramework.h"
#include "Core\FileSystem\FileSystemManager.h"

#include <chrono>
#include <thread>

using namespace XR3D::Core::FileSystem;

namespace
{
    const std::string kIntegDir = "fs_integration_test_data";
}

XR3D_SCENARIO(Integration_FileSystem_BootTimeAssetMountAndLoad)
{
    FileSystemManager::Get().Initialize();

    const std::string assetsRoot = kIntegDir + "/assets";
    FileSystemManager::Get().CreateDirectory(assetsRoot + "/config");
    FileSystemManager::Get().WriteAllText(assetsRoot + "/config/boot.cfg", "window.width = 1920");

    // Simulate the Engine registering its asset mount at boot, the way
    // Main.cpp will once this module is wired in.
    FileSystemManager::Get().RegisterMount("assets", assetsRoot);

    auto bootConfig = FileSystemManager::Get().ReadAllText("assets://config/boot.cfg");
    XR3D_CHECK(bootConfig.success);
    XR3D_CHECK(bootConfig.value.find("window.width") != std::string::npos);

    FileSystemManager::Get().GetRegistry().RemoveMount("assets");
    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_FileSystem_ConfigHotReloadViaWatcher)
{
    FileSystemManager::Get().Initialize();
    FileSystemManager::Get().CreateDirectory(kIntegDir);

    const std::string configPath = kIntegDir + "/hot_reload.cfg";
    FileSystemManager::Get().WriteAllText(configPath, "volume = 0.5");

    // Simulate a system caching the last-loaded content, refreshed only
    // when the watcher reports a change - the pattern Core/Config will
    // use once FileSystem-based hot-reload is wired into it.
    std::string cachedContent;
    auto reload = [&]() {
        auto result = FileSystemManager::Get().ReadAllText(configPath);
        if (result.success)
        {
            cachedContent = result.value;
        }
        };
    reload();
    XR3D_CHECK(cachedContent.find("0.5") != std::string::npos);

    WatchHandle handle = FileSystemManager::Get().Watch(configPath, reload);

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    FileSystemManager::Get().WriteAllText(configPath, "volume = 0.9");
    FileSystemManager::Get().PollWatches();

    XR3D_CHECK(cachedContent.find("0.9") != std::string::npos);

    FileSystemManager::Get().Unwatch(handle);
    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_FileSystem_FirstRunCreatesSaveDirectoryStructure)
{
    const std::string saveRoot = kIntegDir + "/saves/profile1";

    // "Session 1": brand-new install, save directory does not exist yet.
    FileSystemManager::Get().Initialize();
    XR3D_CHECK(!FileSystemManager::Get().Exists(saveRoot));

    FileError mkdirErr = FileSystemManager::Get().CreateDirectory(saveRoot);
    XR3D_CHECK_EQ(static_cast<int>(mkdirErr), static_cast<int>(FileError::None));

    FileError writeErr = FileSystemManager::Get().WriteAllText(saveRoot + "/save1.dat", "level=3;gold=250");
    XR3D_CHECK_EQ(static_cast<int>(writeErr), static_cast<int>(FileError::None));
    FileSystemManager::Get().Shutdown();

    // "Session 2": engine restarts, save data must still be there.
    FileSystemManager::Get().Initialize();
    XR3D_CHECK(FileSystemManager::Get().Exists(saveRoot + "/save1.dat"));

    auto saveData = FileSystemManager::Get().ReadAllText(saveRoot + "/save1.dat");
    XR3D_CHECK(saveData.success);
    XR3D_CHECK(saveData.value.find("gold=250") != std::string::npos);

    FileSystemManager::Get().Shutdown();
    return true;
}