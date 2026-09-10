/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Integration scenarios for Core/Logging: verifies realistic engine-like
* usage (multiple sinks together, high-volume per-frame logging, runtime
* level/category toggling) stays crash-free once Logging is running
* inside the Engine.

*/

#include "IntegrationTestFramework.h"
#include "Core/Logging/Logger.h"

#include <cstdio>
#include <fstream>

using namespace XR3D::Core::Logging;

XR3D_SCENARIO(Integration_Logging_ConsoleAndFileSinkTogether)
{
    Logger::Get().Initialize();

    const std::string path = "xr3d_logging_integration_test.log";
    Logger::Get().GetRegistry().AddSink(std::make_unique<FileSink>(path));

    XR3D_LOG_INFO(LogCategory::Core, "Engine boot sequence starting");
    XR3D_LOG_WARNING(LogCategory::Rendering, "Falling back to software renderer");
    Logger::Get().GetRegistry().FlushAll();

    std::ifstream in(path);
    XR3D_CHECK(in.is_open());
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    XR3D_CHECK(content.find("Engine boot sequence starting") != std::string::npos);
    XR3D_CHECK(content.find("Falling back to software renderer") != std::string::npos);

    in.close();
    std::remove(path.c_str());

    Logger::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_Logging_SimulatedFrameLoop_HighVolume)
{
    Logger::Get().Initialize();

    // Simulate a per-frame log burst across several subsystems, the
    // way a real frame update would touch Rendering/Physics/Audio logs.
    const int kFrameCount = 200;
    for (int frame = 0; frame < kFrameCount; ++frame)
    {
        XR3D_LOG_TRACE(LogCategory::Rendering, "Frame {} draw calls submitted", frame);
        XR3D_LOG_TRACE(LogCategory::Physics, "Frame {} simulation step", frame);
    }

    Logger::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_Logging_RuntimeLevelAndCategoryToggling)
{
    Logger::Get().Initialize();

    // Simulate a developer toggling verbosity at runtime (e.g. via a
    // console command) without restarting the Engine.
    Logger::Get().GetRegistry().SetMinLevel(LogLevel::Warning);
    XR3D_LOG_INFO(LogCategory::Core, "This should be suppressed by min level");
    XR3D_LOG_ERROR(LogCategory::Core, "This should get through");

    Logger::Get().GetRegistry().SetMinLevel(LogLevel::Trace);
    Logger::Get().GetRegistry().SetCategoryEnabled(LogCategory::Audio, false);
    XR3D_LOG_INFO(LogCategory::Audio, "This should be suppressed by category");
    XR3D_LOG_INFO(LogCategory::Core, "This should get through again");

    Logger::Get().Shutdown();
    return true;
}