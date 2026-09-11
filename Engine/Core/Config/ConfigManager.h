/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares ConfigManager, the single entry point for all engine
* configuration access.

*/

#pragma once

#include "ConfigRegistry.h"
#include "ConfigTypes.h"

#include <string>
#include <variant>

namespace XR3D::Core::Config
{
    class ConfigManager
    {
    public:
        static ConfigManager& Get();

        void Initialize();
        void Shutdown();

        bool LoadFromFile(const std::string& filePath);
        bool SaveToFile(const std::string& filePath) const;

        void Set(const std::string& key, ConfigValue value);
        bool Has(const std::string& key) const;

        template <typename T>
        T GetOrDefault(const std::string& key, T defaultValue) const
        {
            auto value = m_registry.Get(key);
            if (!value.has_value())
            {
                return defaultValue;
            }

            if (const T* typed = std::get_if<T>(&value.value()))
            {
                return *typed;
            }

            return defaultValue;
        }

        ConfigRegistry& GetRegistry() { return m_registry; }

    private:
        ConfigManager() = default;
        ~ConfigManager() = default;
        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator=(const ConfigManager&) = delete;

        bool m_initialized = false;
        ConfigRegistry m_registry;

        // TODO (Future): support hot-reload (watch the config file and
        // re-load on change) once Core/FileSystem's file-watcher exists.
    };
}