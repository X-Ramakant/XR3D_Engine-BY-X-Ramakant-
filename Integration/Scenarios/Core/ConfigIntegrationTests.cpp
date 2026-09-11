/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Integration scenarios for Core/Config: verifies realistic engine-like
* usage (boot-time settings load, persisting user settings across
* restarts, graceful first-run behaviour when no config file exists yet).

*/

#include "IntegrationTestFramework.h"
#include "Core\Config\ConfigManager.h"

#include <cstdio>
#include <fstream>

using namespace XR3D::Core::Config;

XR3D_SCENARIO(Integration_Config_LoadEngineSettingsAtStartup)
{
    const std::string path = "xr3d_config_integration_startup.cfg";
    {
        std::ofstream out(path);
        out << "# Engine boot settings\n";
        out << "window.width = 1920\n";
        out << "window.height = 1080\n";
        out << "window.fullscreen = false\n";
        out << "engine.targetFrameRate = 60\n";
    }

    ConfigManager::Get().Initialize();
    XR3D_CHECK(ConfigManager::Get().LoadFromFile(path));

    // Simulate several subsystems reading their own settings at boot,
    // the way Window/Rendering/etc. would once they exist.
    int64_t width = ConfigManager::Get().GetOrDefault<int64_t>("window.width", 1280);
    int64_t height = ConfigManager::Get().GetOrDefault<int64_t>("window.height", 720);
    bool fullscreen = ConfigManager::Get().GetOrDefault<bool>("window.fullscreen", true);
    int64_t targetFps = ConfigManager::Get().GetOrDefault<int64_t>("engine.targetFrameRate", 30);

    XR3D_CHECK_EQ(width, int64_t{ 1920 });
    XR3D_CHECK_EQ(height, int64_t{ 1080 });
    XR3D_CHECK_EQ(fullscreen, false);
    XR3D_CHECK_EQ(targetFps, int64_t{ 60 });

    std::remove(path.c_str());
    ConfigManager::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_Config_PersistUserSettingsAcrossRestarts)
{
    const std::string path = "xr3d_config_integration_persist.cfg";
    std::remove(path.c_str());

    // "Session 1": engine boots with defaults (no file yet), player
    // changes a setting, engine saves on shutdown.
    ConfigManager::Get().Initialize();
    XR3D_CHECK(!ConfigManager::Get().LoadFromFile(path)); // first run, no file yet

    ConfigManager::Get().Set("audio.masterVolume", ConfigValue{ 0.8 });
    ConfigManager::Get().Set("controls.invertY", ConfigValue{ true });
    XR3D_CHECK(ConfigManager::Get().SaveToFile(path));
    ConfigManager::Get().Shutdown();

    // "Session 2": engine restarts, loads the file saved last session.
    ConfigManager::Get().Initialize();
    XR3D_CHECK(ConfigManager::Get().LoadFromFile(path));

    double volume = ConfigManager::Get().GetOrDefault<double>("audio.masterVolume", 1.0);
    bool invertY = ConfigManager::Get().GetOrDefault<bool>("controls.invertY", false);

    XR3D_CHECK_EQ(volume, 0.8);
    XR3D_CHECK_EQ(invertY, true);

    std::remove(path.c_str());
    ConfigManager::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_Config_GracefulFallbackWhenConfigFileMissing)
{
    // Simulates a brand-new install where no config file exists at all -
    // the Engine must still boot cleanly using defaults, not crash.
    ConfigManager::Get().Initialize();

    bool loaded = ConfigManager::Get().LoadFromFile("xr3d_config_never_created.cfg");
    XR3D_CHECK(!loaded);

    int64_t width = ConfigManager::Get().GetOrDefault<int64_t>("window.width", 1280);
    XR3D_CHECK_EQ(width, int64_t{ 1280 });

    ConfigManager::Get().Shutdown();
    return true;
}