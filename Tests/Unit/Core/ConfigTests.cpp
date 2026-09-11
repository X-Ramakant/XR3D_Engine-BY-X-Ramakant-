#include "TestFramework.h"
#include "Core\Config\ConfigManager.h"

#include <cstdio>
#include <fstream>

using namespace XR3D::Core::Config;

XR3D_TEST(Config_Manager_InitializeAndShutdown)
{
    ConfigManager::Get().Initialize();
    ConfigManager::Get().Initialize(); // second call must be a safe no-op
    ConfigManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Config_SetAndGet_ReturnsCorrectTypedValue)
{
    ConfigManager::Get().Initialize();

    ConfigManager::Get().Set("window.width", ConfigValue{ int64_t{1280} });
    ConfigManager::Get().Set("window.fullscreen", ConfigValue{ false });
    ConfigManager::Get().Set("audio.volume", ConfigValue{ 0.5 });
    ConfigManager::Get().Set("engine.name", ConfigValue{ std::string("XR3D") });

    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<int64_t>("window.width", 0), int64_t{ 1280 });
    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<bool>("window.fullscreen", true), false);
    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<double>("audio.volume", 0.0), 0.5);
    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<std::string>("engine.name", ""), std::string("XR3D"));

    ConfigManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Config_GetOrDefault_ReturnsDefaultWhenMissing)
{
    ConfigManager::Get().Initialize();

    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<int64_t>("does.not.exist", 99), int64_t{ 99 });
    XR3D_CHECK(!ConfigManager::Get().Has("does.not.exist"));

    ConfigManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Config_GetOrDefault_ReturnsDefaultWhenTypeMismatch)
{
    ConfigManager::Get().Initialize();

    ConfigManager::Get().Set("window.width", ConfigValue{ int64_t{1920} });

    // Asking for a bool when the stored value is an int must fall back
    // to the default, not crash or silently reinterpret the value.
    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<bool>("window.width", true), true);

    ConfigManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Config_Registry_HasAndRemove)
{
    ConfigManager::Get().Initialize();

    ConfigManager::Get().Set("temp.key", ConfigValue{ true });
    XR3D_CHECK(ConfigManager::Get().Has("temp.key"));

    ConfigManager::Get().GetRegistry().Remove("temp.key");
    XR3D_CHECK(!ConfigManager::Get().Has("temp.key"));

    ConfigManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Config_SaveAndLoad_RoundTrip)
{
    ConfigManager::Get().Initialize();

    ConfigManager::Get().Set("window.width", ConfigValue{ int64_t{1600} });
    ConfigManager::Get().Set("window.fullscreen", ConfigValue{ true });
    ConfigManager::Get().Set("physics.gravity", ConfigValue{ -9.8 });
    ConfigManager::Get().Set("player.name", ConfigValue{ std::string("Ramakant") });

    const std::string path = "xr3d_config_unit_test.cfg";
    XR3D_CHECK(ConfigManager::Get().SaveToFile(path));

    ConfigManager::Get().Shutdown();
    ConfigManager::Get().Initialize();
    XR3D_CHECK_EQ(ConfigManager::Get().GetRegistry().GetCount(), size_t{ 0 });

    XR3D_CHECK(ConfigManager::Get().LoadFromFile(path));
    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<int64_t>("window.width", 0), int64_t{ 1600 });
    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<bool>("window.fullscreen", false), true);
    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<double>("physics.gravity", 0.0), -9.8);
    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<std::string>("player.name", ""), std::string("Ramakant"));

    std::remove(path.c_str());
    ConfigManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Config_Loader_SkipsCommentsAndBlankLines)
{
    const std::string path = "xr3d_config_comments_test.cfg";
    {
        std::ofstream out(path);
        out << "# a comment\n";
        out << "\n";
        out << "// another comment style\n";
        out << "this_line_has_no_equals_sign\n";
        out << "valid.key = 42\n";
    }

    ConfigManager::Get().Initialize();
    XR3D_CHECK(ConfigManager::Get().LoadFromFile(path));
    XR3D_CHECK_EQ(ConfigManager::Get().GetOrDefault<int64_t>("valid.key", 0), int64_t{ 42 });
    XR3D_CHECK(!ConfigManager::Get().Has("this_line_has_no_equals_sign"));

    std::remove(path.c_str());
    ConfigManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Config_Loader_HandlesMissingFileGracefully)
{
    ConfigManager::Get().Initialize();
    XR3D_CHECK(!ConfigManager::Get().LoadFromFile("xr3d_this_file_does_not_exist.cfg"));
    ConfigManager::Get().Shutdown();
    return true;
}