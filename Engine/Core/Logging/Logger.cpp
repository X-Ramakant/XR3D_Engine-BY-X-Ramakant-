/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements Logger.

*/

#include "Logger.h"

namespace XR3D::Core::Logging
{
    Logger& Logger::Get()
    {
        static Logger instance;
        return instance;
    }

    void Logger::Initialize()
    {
        if (m_initialized)
        {
            return;
        }

        m_formatter = std::make_unique<DefaultLogFormatter>();
        m_registry.AddSink(std::make_unique<ConsoleSink>());

        m_initialized = true;
    }

    void Logger::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        m_registry.FlushAll();
        m_registry.RemoveAllSinks();
        m_formatter.reset();

        m_initialized = false;
    }

    void Logger::Log(LogLevel level, LogCategory category, const std::string& message,
        const char* file, int line)
    {
        if (!m_initialized)
        {
            return;
        }

        LogMessage logMessage;
        logMessage.level = level;
        logMessage.category = category;
        logMessage.text = message;
        logMessage.sourceFile = file;
        logMessage.sourceLine = line;

        m_registry.Dispatch(logMessage, *m_formatter);
    }
}