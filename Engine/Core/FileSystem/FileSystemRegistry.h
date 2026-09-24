/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares FileSystemRegistry, which maps virtual "alias://" paths to
* real on-disk paths (e.g. "assets://textures/wall.png").

*/

#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

namespace XR3D::Core::FileSystem
{
    class FileSystemRegistry
    {
    public:
        void RegisterMount(const std::string& alias, const std::string& realPath);
        void RemoveMount(const std::string& alias);
        bool HasMount(const std::string& alias) const;

        // Resolves "alias://rest/of/path" into a real filesystem path.
        // If the input has no "://" separator, or the alias is unknown,
        // it is returned unchanged - this makes ResolvePath safe to call
        // unconditionally on ANY path (already-real paths pass through).
        std::string ResolvePath(const std::string& virtualPath) const;

    private:
        mutable std::mutex m_mutex;
        std::unordered_map<std::string, std::string> m_mounts;
    };
}