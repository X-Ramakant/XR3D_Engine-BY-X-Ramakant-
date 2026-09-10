/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements DefaultLogFormatter.

*/

#include "LogFormatter.h"

#include <ctime>
#include <sstream>
#include <iomanip>

namespace XR3D::Core::Logging
{
    std::string DefaultLogFormatter::Format(const LogMessage& message) const
    {
        std::time_t timeT = std::chrono::system_clock::to_time_t(message.timestamp);
        std::tm localTime{};

#if defined(_WIN32)
        localtime_s(&localTime, &timeT);
#else
        localtime_r(&timeT, &localTime);
#endif

        std::ostringstream out;
        out << '[' << std::put_time(&localTime, "%H:%M:%S") << "] "
            << '[' << LogLevelToString(message.level) << "] "
            << '[' << LogCategoryToString(message.category) << "] "
            << message.text;

        if (message.sourceFile != nullptr)
        {
            out << " (" << message.sourceFile << ':' << message.sourceLine << ')';
        }

        return out.str();
    }
}