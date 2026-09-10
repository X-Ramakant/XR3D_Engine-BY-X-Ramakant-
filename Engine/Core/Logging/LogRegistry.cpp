/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements LogRegistry.

*/

#include "LogRegistry.h"
#include "LogFormatter.h"

namespace XR3D::Core::Logging
{
    LogRegistry::LogRegistry()
    {
        m_categoryEnabled.fill(true);
    }

    void LogRegistry::AddSink(std::unique_ptr<ISink> sink)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sinks.push_back(std::move(sink));
    }

    void LogRegistry::RemoveAllSinks()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sinks.clear();
    }

    void LogRegistry::SetMinLevel(LogLevel level)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_minLevel = level;
    }

    LogLevel LogRegistry::GetMinLevel() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_minLevel;
    }

    void LogRegistry::SetCategoryEnabled(LogCategory category, bool enabled)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_categoryEnabled[static_cast<size_t>(category)] = enabled;
    }

    bool LogRegistry::IsCategoryEnabled(LogCategory category) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_categoryEnabled[static_cast<size_t>(category)];
    }

    void LogRegistry::Dispatch(const LogMessage& message, const ILogFormatter& formatter)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (message.level < m_minLevel)
        {
            return;
        }

        if (!m_categoryEnabled[static_cast<size_t>(message.category)])
        {
            return;
        }

        const std::string formatted = formatter.Format(message);

        for (auto& sink : m_sinks)
        {
            sink->Write(formatted, message.level);
        }
    }

    void LogRegistry::FlushAll()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& sink : m_sinks)
        {
            sink->Flush();
        }
    }
}