/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements FileSystemRegistry.

*/

#include "FileSystemRegistry.h"
#include "FileSystemPath.h"

namespace XR3D::Core::FileSystem
{
    void FileSystemRegistry::RegisterMount(const std::string& alias, const std::string& realPath)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_mounts[alias] = realPath;
    }

    void FileSystemRegistry::RemoveMount(const std::string& alias)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_mounts.erase(alias);
    }

    bool FileSystemRegistry::HasMount(const std::string& alias) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_mounts.find(alias) != m_mounts.end();
    }

    std::string FileSystemRegistry::ResolvePath(const std::string& virtualPath) const
    {
        const std::string separator = "://";
        const size_t sepPos = virtualPath.find(separator);

        if (sepPos == std::string::npos)
        {
            return virtualPath; // Not a virtual path - pass through unchanged.
        }

        const std::string alias = virtualPath.substr(0, sepPos);
        const std::string rest = virtualPath.substr(sepPos + separator.size());

        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_mounts.find(alias);
        if (it == m_mounts.end())
        {
            // Unknown alias - pass through unchanged rather than fail
            // here. The caller's Exists()/ReadAllText() will naturally
            // report NotFound when it tries the literal string as a path.
            return virtualPath;
        }

        return FileSystemPath::Join(it->second, rest);
    }
}