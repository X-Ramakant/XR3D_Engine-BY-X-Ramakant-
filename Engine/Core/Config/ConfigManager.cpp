/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements ConfigManager.

*/

#include "ConfigManager.h"
#include "ConfigLoader.h"

namespace XR3D::Core::Config
{
    ConfigManager& ConfigManager::Get()
    {
        static ConfigManager instance;
        return instance;
    }

    void ConfigManager::Initialize()
    {
        if (m_initialized)
        {
            return;
        }

        m_initialized = true;
    }

    void ConfigManager::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        m_registry.Clear();
        m_initialized = false;
    }

    bool ConfigManager::LoadFromFile(const std::string& filePath)
    {
        return ConfigLoader::LoadFromFile(filePath, m_registry);
    }

    bool ConfigManager::SaveToFile(const std::string& filePath) const
    {
        return ConfigLoader::SaveToFile(filePath, m_registry);
    }

    void ConfigManager::Set(const std::string& key, ConfigValue value)
    {
        m_registry.Set(key, std::move(value));
    }

    bool ConfigManager::Has(const std::string& key) const
    {
        return m_registry.Has(key);
    }
}