/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements FileSystemManager.

*/

#include "FileSystemManager.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace XR3D::Core::FileSystem
{
    namespace
    {
        FileError MapErrorCode(const std::error_code& ec)
        {
            if (!ec)
            {
                return FileError::None;
            }
            if (ec == std::errc::no_such_file_or_directory)
            {
                return FileError::NotFound;
            }
            if (ec == std::errc::permission_denied)
            {
                return FileError::AccessDenied;
            }
            if (ec == std::errc::file_exists)
            {
                return FileError::AlreadyExists;
            }
            return FileError::IOError;
        }
    }

    FileSystemManager& FileSystemManager::Get()
    {
        static FileSystemManager instance;
        return instance;
    }

    void FileSystemManager::Initialize()
    {
        if (m_initialized)
        {
            return;
        }

        m_initialized = true;
    }

    void FileSystemManager::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        m_initialized = false;
    }

    std::string FileSystemManager::Resolve(const std::string& path) const
    {
        return m_registry.ResolvePath(path);
    }

    FileResult<std::string> FileSystemManager::ReadAllText(const std::string& path)
    {
        const std::string realPath = Resolve(path);

        std::error_code ec;
        if (!fs::exists(realPath, ec))
        {
            return FileResult<std::string>::Fail(FileError::NotFound);
        }

        std::ifstream file(realPath, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            return FileResult<std::string>::Fail(FileError::IOError);
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        if (file.bad())
        {
            return FileResult<std::string>::Fail(FileError::IOError);
        }

        return FileResult<std::string>::Ok(std::move(content));
    }

    FileError FileSystemManager::WriteAllText(const std::string& path, const std::string& content)
    {
        const std::string realPath = Resolve(path);

        std::ofstream file(realPath, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open())
        {
            return FileError::IOError;
        }

        file << content;
        if (file.bad())
        {
            return FileError::IOError;
        }

        return FileError::None;
    }

    FileResult<std::vector<uint8_t>> FileSystemManager::ReadAllBytes(const std::string& path)
    {
        const std::string realPath = Resolve(path);

        std::error_code ec;
        if (!fs::exists(realPath, ec))
        {
            return FileResult<std::vector<uint8_t>>::Fail(FileError::NotFound);
        }

        std::ifstream file(realPath, std::ios::in | std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            return FileResult<std::vector<uint8_t>>::Fail(FileError::IOError);
        }

        const std::streamsize size = file.tellg();
        if (size < 0)
        {
            return FileResult<std::vector<uint8_t>>::Fail(FileError::IOError);
        }
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(static_cast<size_t>(size));
        if (size > 0 && !file.read(reinterpret_cast<char*>(buffer.data()), size))
        {
            return FileResult<std::vector<uint8_t>>::Fail(FileError::IOError);
        }

        return FileResult<std::vector<uint8_t>>::Ok(std::move(buffer));
    }

    FileError FileSystemManager::WriteAllBytes(const std::string& path, const std::vector<uint8_t>& data)
    {
        const std::string realPath = Resolve(path);

        std::ofstream file(realPath, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open())
        {
            return FileError::IOError;
        }

        if (!data.empty())
        {
            file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        }

        if (file.bad())
        {
            return FileError::IOError;
        }

        return FileError::None;
    }

    bool FileSystemManager::Exists(const std::string& path)
    {
        std::error_code ec;
        return fs::exists(Resolve(path), ec);
    }

    bool FileSystemManager::IsDirectory(const std::string& path)
    {
        std::error_code ec;
        return fs::is_directory(Resolve(path), ec);
    }

    FileError FileSystemManager::CreateDirectory(const std::string& path)
    {
        std::error_code ec;
        fs::create_directories(Resolve(path), ec);
        return MapErrorCode(ec);
    }

    FileError FileSystemManager::DeleteFile(const std::string& path)
    {
        std::error_code ec;
        const bool removed = fs::remove(Resolve(path), ec);

        if (ec)
        {
            return MapErrorCode(ec);
        }
        if (!removed)
        {
            return FileError::NotFound;
        }
        return FileError::None;
    }

    FileResult<std::vector<std::string>> FileSystemManager::ListDirectory(const std::string& path)
    {
        const std::string realPath = Resolve(path);

        std::error_code ec;
        if (!fs::is_directory(realPath, ec))
        {
            return FileResult<std::vector<std::string>>::Fail(FileError::NotFound);
        }

        std::vector<std::string> entries;
        for (const auto& entry : fs::directory_iterator(realPath, ec))
        {
            entries.push_back(entry.path().filename().string());
        }

        if (ec)
        {
            return FileResult<std::vector<std::string>>::Fail(FileError::IOError);
        }

        return FileResult<std::vector<std::string>>::Ok(std::move(entries));
    }

    FileResult<FileInfo> FileSystemManager::GetInfo(const std::string& path)
    {
        const std::string realPath = Resolve(path);

        std::error_code ec;
        if (!fs::exists(realPath, ec))
        {
            return FileResult<FileInfo>::Fail(FileError::NotFound);
        }

        FileInfo info;
        info.path = realPath;
        info.isDirectory = fs::is_directory(realPath, ec);

        if (!info.isDirectory)
        {
            info.size = fs::file_size(realPath, ec);
        }

        return FileResult<FileInfo>::Ok(std::move(info));
    }

    void FileSystemManager::RegisterMount(const std::string& alias, const std::string& realPath)
    {
        m_registry.RegisterMount(alias, realPath);
    }

    bool FileSystemManager::HasMount(const std::string& alias) const
    {
        return m_registry.HasMount(alias);
    }

    WatchHandle FileSystemManager::Watch(const std::string& path, std::function<void()> onChanged)
    {
        return m_watcher.Watch(Resolve(path), std::move(onChanged));
    }

    void FileSystemManager::Unwatch(WatchHandle handle)
    {
        m_watcher.Unwatch(handle);
    }

    void FileSystemManager::PollWatches()
    {
        m_watcher.PollAll();
    }

}