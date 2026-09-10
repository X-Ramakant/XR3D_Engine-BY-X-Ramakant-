/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares Logger, the single entry point for all engine logging.
* Owns the LogRegistry and the active LogFormatter.

*/

#pragma once

#include "LogFormatter.h"
#include "LogRegistry.h"
#include "LogTypes.h"

#include <format>
#include <memory>
#include <string>

namespace XR3D::Core::Logging
{
    class Logger
    {
    public:
        static Logger& Get();

        void Initialize();
        void Shutdown();

        void Log(LogLevel level, LogCategory category, const std::string& message,
            const char* file, int line);

        LogRegistry& GetRegistry() { return m_registry; }

    private:
        Logger() = default;
        ~Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        bool m_initialized = false;
        LogRegistry m_registry;
        std::unique_ptr<ILogFormatter> m_formatter;

        // TODO (Future): route Fatal-level logs through Core/Diagnostics
        // (e.g. trigger a crash report) once that system exists.
    };
}

#define XR3D_LOG(level, category, fmt, ...) \
    ::XR3D::Core::Logging::Logger::Get().Log((level), (category), \
        std::format(fmt, __VA_ARGS__), __FILE__, __LINE__)

#define XR3D_LOG_TRACE(category, fmt, ...)   XR3D_LOG(::XR3D::Core::Logging::LogLevel::Trace,   category, fmt, __VA_ARGS__)
#define XR3D_LOG_DEBUG(category, fmt, ...)   XR3D_LOG(::XR3D::Core::Logging::LogLevel::Debug,   category, fmt, __VA_ARGS__)
#define XR3D_LOG_INFO(category, fmt, ...)    XR3D_LOG(::XR3D::Core::Logging::LogLevel::Info,    category, fmt, __VA_ARGS__)
#define XR3D_LOG_WARNING(category, fmt, ...) XR3D_LOG(::XR3D::Core::Logging::LogLevel::Warning, category, fmt, __VA_ARGS__)
#define XR3D_LOG_ERROR(category, fmt, ...)   XR3D_LOG(::XR3D::Core::Logging::LogLevel::Error,   category, fmt, __VA_ARGS__)
#define XR3D_LOG_FATAL(category, fmt, ...)   XR3D_LOG(::XR3D::Core::Logging::LogLevel::Fatal,   category, fmt, __VA_ARGS__)