/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares LogRegistry, which owns all sinks, the global minimum log
* level, and per-category enable/disable filtering.

*/

#pragma once

#include "LogSink.h"
#include "LogTypes.h"

#include <array>
#include <memory>
#include <mutex>
#include <vector>

namespace XR3D::Core::Logging
{
    class ILogFormatter;

    class LogRegistry
    {
    public:
        LogRegistry();

        void AddSink(std::unique_ptr<ISink> sink);
        void RemoveAllSinks();

        void SetMinLevel(LogLevel level);
        LogLevel GetMinLevel() const;

        void SetCategoryEnabled(LogCategory category, bool enabled);
        bool IsCategoryEnabled(LogCategory category) const;

        // Formats the message once, then writes it to every registered
        // sink that passes the level/category filters.
        void Dispatch(const LogMessage& message, const ILogFormatter& formatter);

        void FlushAll();

    private:
        mutable std::mutex m_mutex;
        std::vector<std::unique_ptr<ISink>> m_sinks;
        LogLevel m_minLevel = LogLevel::Trace;
        std::array<bool, static_cast<size_t>(LogCategory::Count)> m_categoryEnabled;

        // TODO (Future): add per-sink minimum level (e.g. Console shows
        // Info+, File captures everything) once a real use case needs it.
    };
}