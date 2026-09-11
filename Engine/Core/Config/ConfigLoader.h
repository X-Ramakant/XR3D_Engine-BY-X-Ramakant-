/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares ConfigLoader, which reads/writes a simple "key = value" text
* config format into/out of a ConfigRegistry.

*/

#pragma once

#include "ConfigRegistry.h"
#include "ConfigTypes.h"

#include <string>

namespace XR3D::Core::Config
{
    // File format (one entry per line):
    //   # this is a comment
    //   // this is also a comment
    //   window.width = 1920
    //   window.fullscreen = true
    //   engine.version = "1.0.0"
    //
    // TODO (Future): route file access through Core/FileSystem once that
    // system exists, instead of std::ifstream/std::ofstream directly.
    class ConfigLoader
    {
    public:
        static bool LoadFromFile(const std::string& filePath, ConfigRegistry& outRegistry);
        static bool SaveToFile(const std::string& filePath, const ConfigRegistry& registry);

        static ConfigValue ParseValue(const std::string& rawValue);
        static std::string SerializeValue(const ConfigValue& value);
    };
}