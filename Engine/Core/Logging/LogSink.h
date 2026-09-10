/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares ISink and the built-in ConsoleSink / FileSink output targets.

*/

#pragma once

#include "LogTypes.h"

#include <fstream>
#include <string>

namespace XR3D::Core::Logging
{
    class ISink
    {
    public:
        virtual ~ISink() = default;
        virtual void Write(const std::string& formattedMessage, LogLevel level) = 0;
        virtual void Flush() {}
        virtual const char* GetName() const = 0;
    };

    // Writes to stdout (or stderr for Error/Fatal). Always available.
    class ConsoleSink final : public ISink
    {
    public:
        void Write(const std::string& formattedMessage, LogLevel level) override;
        void Flush() override;
        const char* GetName() const override { return "ConsoleSink"; }
    };

    // Writes to a plain text file on disk.
    // TODO (Future): route through Core/FileSystem once that system
    // exists, instead of opening std::ofstream directly.
    class FileSink final : public ISink
    {
    public:
        explicit FileSink(const std::string& filePath);
        ~FileSink() override;

        void Write(const std::string& formattedMessage, LogLevel level) override;
        void Flush() override;
        const char* GetName() const override { return "FileSink"; }

        bool IsOpen() const;

    private:
        std::ofstream m_file;
    };

    // TODO (Future): add a PlatformDebugSink (Visual Studio Output window)
    // once Platform/Windows exists.
}