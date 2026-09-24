/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements FileSystemPath.

*/

#include "FileSystemPath.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace XR3D::Core::FileSystem
{
    std::string FileSystemPath::Normalize(const std::string& path)
    {
        return fs::path(path).lexically_normal().generic_string();
    }

    std::string FileSystemPath::Join(const std::string& a, const std::string& b)
    {
        fs::path result = fs::path(a) / fs::path(b);
        return result.lexically_normal().generic_string();
    }

    std::string FileSystemPath::GetExtension(const std::string& path)
    {
        return fs::path(path).extension().string();
    }

    std::string FileSystemPath::GetFileName(const std::string& path)
    {
        return fs::path(path).filename().string();
    }

    std::string FileSystemPath::GetFileNameWithoutExtension(const std::string& path)
    {
        return fs::path(path).stem().string();
    }

    std::string FileSystemPath::GetDirectory(const std::string& path)
    {
        return fs::path(path).parent_path().generic_string();
    }

    bool FileSystemPath::IsAbsolute(const std::string& path)
    {
        return fs::path(path).is_absolute();
    }
}