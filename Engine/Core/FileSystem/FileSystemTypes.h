/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Defines shared types used across the FileSystem system: FileError,
* the generic FileResult<T> wrapper (no exceptions thrown anywhere in
* this module), FileInfo, and the generational WatchHandle.

*/

#pragma once

#include <cstdint>
#include <string>

namespace XR3D::Core::FileSystem
{
    enum class FileError : uint8_t
    {
        None = 0,
        NotFound,
        AccessDenied,
        AlreadyExists,
        InvalidPath,
        IOError,
        Unknown
    };

    constexpr const char* FileErrorToString(FileError error)
    {
        switch (error)
        {
        case FileError::None:          return "None";
        case FileError::NotFound:      return "NotFound";
        case FileError::AccessDenied:  return "AccessDenied";
        case FileError::AlreadyExists: return "AlreadyExists";
        case FileError::InvalidPath:   return "InvalidPath";
        case FileError::IOError:       return "IOError";
        default:                       return "Unknown";
        }
    }

    // Every read-style operation returns this instead of throwing.
    // Callers MUST check `success` before touching `value`.
    template <typename T>
    struct FileResult
    {
        bool success = false;
        FileError error = FileError::Unknown;
        T value{};

        static FileResult<T> Ok(T v)
        {
            FileResult<T> result;
            result.success = true;
            result.error = FileError::None;
            result.value = std::move(v);
            return result;
        }

        static FileResult<T> Fail(FileError e)
        {
            FileResult<T> result;
            result.success = false;
            result.error = e;
            return result;
        }
    };

    struct FileInfo
    {
        std::string path;
        uintmax_t size = 0;
        bool isDirectory = false;
    };

    // Generational handle for file watches - same safety pattern as
    // Core/Time's TimerHandle (index alone would risk a stale reference
    // after a slot is reused).
    struct WatchHandle
    {
        static constexpr uint32_t kInvalidIndex = 0xFFFFFFFFu;

        uint32_t index = kInvalidIndex;
        uint32_t generation = 0;

        bool IsValid() const { return index != kInvalidIndex; }

        bool operator==(const WatchHandle& other) const
        {
            return index == other.index && generation == other.generation;
        }

        bool operator!=(const WatchHandle& other) const
        {
            return !(*this == other);
        }
    };

    inline constexpr WatchHandle kInvalidWatchHandle{};
}