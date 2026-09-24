/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares FileSystemPath, cross-platform path manipulation built on
* std::filesystem. Pure string manipulation only - no disk access.

*/

#pragma once

#include <string>

namespace XR3D::Core::FileSystem
{
    class FileSystemPath
    {
    public:
        // Collapses "." / ".." segments and always returns forward
        // slashes, regardless of host OS, so paths are comparable and
        // loggable consistently across Windows/Linux/macOS.
        static std::string Normalize(const std::string& path);

        static std::string Join(const std::string& a, const std::string& b);
        static std::string GetExtension(const std::string& path);
        static std::string GetFileName(const std::string& path);
        static std::string GetFileNameWithoutExtension(const std::string& path);
        static std::string GetDirectory(const std::string& path);
        static bool IsAbsolute(const std::string& path);
    };
}