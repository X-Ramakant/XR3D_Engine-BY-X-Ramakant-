/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares ILogFormatter and DefaultLogFormatter, which convert a
* LogMessage into a final printable string.

*/

#pragma once

#include "LogTypes.h"

namespace XR3D::Core::Logging
{
    class ILogFormatter
    {
    public:
        virtual ~ILogFormatter() = default;
        virtual std::string Format(const LogMessage& message) const = 0;
    };

    // Produces lines like:
    // [12:34:56] [INFO] [Rendering] Shader compiled (Renderer.cpp:42)
    class DefaultLogFormatter final : public ILogFormatter
    {
    public:
        std::string Format(const LogMessage& message) const override;
    };

    // TODO (Future): add a JsonLogFormatter once external log aggregation
    // / crash-reporting tools need structured output instead of plain text.
}