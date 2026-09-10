/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements ConsoleSink and FileSink.

*/

#include "LogSink.h"

#include <cstdio>

namespace XR3D::Core::Logging
{
    // ---------------- ConsoleSink ----------------

    void ConsoleSink::Write(const std::string& formattedMessage, LogLevel level)
    {
        if (level == LogLevel::Error || level == LogLevel::Fatal)
        {
            std::fprintf(stderr, "%s\n", formattedMessage.c_str());
        }
        else
        {
            std::fprintf(stdout, "%s\n", formattedMessage.c_str());
        }
    }

    void ConsoleSink::Flush()
    {
        std::fflush(stdout);
        std::fflush(stderr);
    }

    // ---------------- FileSink ----------------

    FileSink::FileSink(const std::string& filePath)
        : m_file(filePath, std::ios::out | std::ios::app)
    {
    }

    FileSink::~FileSink()
    {
        if (m_file.is_open())
        {
            m_file.flush();
            m_file.close();
        }
    }

    void FileSink::Write(const std::string& formattedMessage, LogLevel level)
    {
        (void)level;

        if (m_file.is_open())
        {
            m_file << formattedMessage << '\n';
        }
    }

    void FileSink::Flush()
    {
        if (m_file.is_open())
        {
            m_file.flush();
        }
    }

    bool FileSink::IsOpen() const
    {
        return m_file.is_open();
    }
}