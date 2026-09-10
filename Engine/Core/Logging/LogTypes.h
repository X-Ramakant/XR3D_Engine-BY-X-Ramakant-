/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Defines shared types, enums and helper functions used across the
* Logging system.

*/

#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace XR3D::Core::Logging
{
    enum class LogLevel : uint8_t
    {
        Trace = 0,
        Debug,
        Info,
        Warning,
        Error,
        Fatal,
        Count
    };

    enum class LogCategory : uint8_t
    {
        Untagged = 0,
        Core,
        Rendering,
        Physics,
        Audio,
        Scripting,
        Networking,
        UI,
        AI,
        Resource,
        Count
    };

    struct LogMessage
    {
        LogLevel    level = LogLevel::Info;
        LogCategory category = LogCategory::Untagged;
        std::string text;
        const char* sourceFile = nullptr;
        int         sourceLine = 0;
        std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
    };

    constexpr const char* LogLevelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Trace:   return "TRACE";
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
        default:                return "UNKNOWN";
        }
    }

    constexpr const char* LogCategoryToString(LogCategory category)
    {
        switch (category)
        {
        case LogCategory::Untagged:   return "Untagged";
        case LogCategory::Core:       return "Core";
        case LogCategory::Rendering:  return "Rendering";
        case LogCategory::Physics:    return "Physics";
        case LogCategory::Audio:      return "Audio";
        case LogCategory::Scripting:  return "Scripting";
        case LogCategory::Networking: return "Networking";
        case LogCategory::UI:         return "UI";
        case LogCategory::AI:         return "AI";
        case LogCategory::Resource:   return "Resource";
        default:                      return "Unknown";
        }
    }

    // TODO (Future): add a MemoryTag <-> LogCategory mapping helper if the
    // two taxonomies need to stay in sync once more subsystems exist.
}