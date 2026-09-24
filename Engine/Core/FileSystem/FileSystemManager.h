/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares FileSystemManager, the single entry point for all engine
* file I/O, path resolution, and file watching. No function in this
* class throws - every failure is reported via FileError/FileResult.

*/

#pragma once

#include "FileSystemPath.h"
#include "FileSystemRegistry.h"
#include "FileSystemTypes.h"
#include "FileSystemWatcher.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace XR3D::Core::FileSystem
{
    class FileSystemManager
    {
    public:
        static FileSystemManager& Get();

        void Initialize();
        void Shutdown();

        FileResult<std::string> ReadAllText(const std::string& path);
        FileError WriteAllText(const std::string& path, const std::string& content);

        FileResult<std::vector<uint8_t>> ReadAllBytes(const std::string& path);
        FileError WriteAllBytes(const std::string& path, const std::vector<uint8_t>& data);

        bool Exists(const std::string& path);
        bool IsDirectory(const std::string& path);
        FileError CreateDirectory(const std::string& path);
        FileError DeleteFile(const std::string& path);
        FileResult<std::vector<std::string>> ListDirectory(const std::string& path);
        FileResult<FileInfo> GetInfo(const std::string& path);

        // Virtual mount points (e.g. "assets://"), delegates to FileSystemRegistry.
        void RegisterMount(const std::string& alias, const std::string& realPath);
        bool HasMount(const std::string& alias) const;

        // File watching, delegates to FileSystemWatcher. Call
        // PollWatches() once per frame/tick to check for changes.
        WatchHandle Watch(const std::string& path, std::function<void()> onChanged);
        void Unwatch(WatchHandle handle);
        void PollWatches();

        FileSystemRegistry& GetRegistry() { return m_registry; }
        FileSystemWatcher& GetWatcher() { return m_watcher; }

    private:
        FileSystemManager() = default;
        ~FileSystemManager() = default;
        FileSystemManager(const FileSystemManager&) = delete;
        FileSystemManager& operator=(const FileSystemManager&) = delete;

        std::string Resolve(const std::string& path) const;

        bool m_initialized = false;
        FileSystemRegistry m_registry;
        FileSystemWatcher m_watcher;
    };
}