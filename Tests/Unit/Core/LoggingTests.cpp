#include "TestFramework.h"
#include "Core\Logging\Logger.h"

#include <cstdio>
#include <fstream>
#include <sstream>

using namespace XR3D::Core::Logging;

namespace
{
    // A local test-only sink that just records what it receives,
    // so filtering behavior can be asserted without touching stdout.
    class RecordingSink final : public ISink
    {
    public:
        void Write(const std::string& formattedMessage, LogLevel level) override
        {
            (void)level;
            lastMessage = formattedMessage;
            ++writeCount;
        }
        void Flush() override {}
        const char* GetName() const override { return "RecordingSink"; }

        std::string lastMessage;
        int writeCount = 0;
    };
}

XR3D_TEST(Logging_Logger_InitializeAndShutdown)
{
    Logger::Get().Initialize();
    Logger::Get().Initialize(); // second call must be a safe no-op
    Logger::Get().Shutdown();
    return true;
}

XR3D_TEST(Logging_DefaultFormatter_ContainsLevelCategoryAndText)
{
    LogMessage message;
    message.level = LogLevel::Warning;
    message.category = LogCategory::Rendering;
    message.text = "shader cache miss";
    message.sourceFile = "Renderer.cpp";
    message.sourceLine = 42;

    DefaultLogFormatter formatter;
    std::string formatted = formatter.Format(message);

    XR3D_CHECK(formatted.find("WARNING") != std::string::npos);
    XR3D_CHECK(formatted.find("Rendering") != std::string::npos);
    XR3D_CHECK(formatted.find("shader cache miss") != std::string::npos);
    XR3D_CHECK(formatted.find("Renderer.cpp:42") != std::string::npos);
    return true;
}

XR3D_TEST(Logging_FileSink_WritesToFile)
{
    const std::string path = "xr3d_logging_unit_test.log";

    {
        FileSink sink(path);
        XR3D_CHECK(sink.IsOpen());
        sink.Write("hello from unit test", LogLevel::Info);
        sink.Flush();
    }

    std::ifstream in(path);
    XR3D_CHECK(in.is_open());

    std::string line;
    std::getline(in, line);
    XR3D_CHECK_EQ(line, std::string("hello from unit test"));

    in.close();
    std::remove(path.c_str());
    return true;
}

XR3D_TEST(Logging_Registry_CategoryFilter_BlocksDisabledCategory)
{
    LogRegistry registry;
    DefaultLogFormatter formatter;

    auto recordingSinkOwned = std::make_unique<RecordingSink>();
    RecordingSink* recording = recordingSinkOwned.get();
    registry.AddSink(std::move(recordingSinkOwned));

    registry.SetCategoryEnabled(LogCategory::Audio, false);

    LogMessage blocked;
    blocked.category = LogCategory::Audio;
    blocked.text = "should not appear";
    registry.Dispatch(blocked, formatter);

    XR3D_CHECK_EQ(recording->writeCount, 0);

    LogMessage allowed;
    allowed.category = LogCategory::Rendering;
    allowed.text = "should appear";
    registry.Dispatch(allowed, formatter);

    XR3D_CHECK_EQ(recording->writeCount, 1);
    return true;
}

XR3D_TEST(Logging_Registry_LevelFilter_BlocksBelowMinLevel)
{
    LogRegistry registry;
    DefaultLogFormatter formatter;

    auto recordingSinkOwned = std::make_unique<RecordingSink>();
    RecordingSink* recording = recordingSinkOwned.get();
    registry.AddSink(std::move(recordingSinkOwned));

    registry.SetMinLevel(LogLevel::Error);

    LogMessage infoMessage;
    infoMessage.level = LogLevel::Info;
    infoMessage.text = "should be filtered out";
    registry.Dispatch(infoMessage, formatter);

    XR3D_CHECK_EQ(recording->writeCount, 0);

    LogMessage errorMessage;
    errorMessage.level = LogLevel::Error;
    errorMessage.text = "should pass through";
    registry.Dispatch(errorMessage, formatter);

    XR3D_CHECK_EQ(recording->writeCount, 1);
    return true;
}

XR3D_TEST(Logging_Macro_FormatsArgumentsCorrectly)
{
    Logger::Get().Initialize();
    Logger::Get().GetRegistry().RemoveAllSinks();

    auto recordingSinkOwned = std::make_unique<RecordingSink>();
    RecordingSink* recording = recordingSinkOwned.get();
    Logger::Get().GetRegistry().AddSink(std::move(recordingSinkOwned));

    XR3D_LOG_INFO(LogCategory::Core, "frame={} deltaTime={}", 7, 0.016);
 
    XR3D_CHECK(recording->lastMessage.find("frame=7") != std::string::npos);
    XR3D_CHECK(recording->lastMessage.find("0.016") != std::string::npos);

    Logger::Get().Shutdown();
    return true;
}