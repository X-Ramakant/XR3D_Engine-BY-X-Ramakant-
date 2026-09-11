/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements ConfigRegistry.

*/

#include "ConfigRegistry.h"

namespace XR3D::Core::Config
{
    void ConfigRegistry::Set(const std::string& key, ConfigValue value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_values[key] = std::move(value);
    }

    bool ConfigRegistry::Has(const std::string& key) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_values.find(key) != m_values.end();
    }

    std::optional<ConfigValue> ConfigRegistry::Get(const std::string& key) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_values.find(key);
        if (it == m_values.end())
        {
            return std::nullopt;
        }

        return it->second;
    }

    void ConfigRegistry::Remove(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_values.erase(key);
    }

    void ConfigRegistry::Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_values.clear();
    }

    std::vector<std::string> ConfigRegistry::GetAllKeys() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::vector<std::string> keys;
        keys.reserve(m_values.size());
        for (const auto& pair : m_values)
        {
            keys.push_back(pair.first);
        }
        return keys;
    }

    size_t ConfigRegistry::GetCount() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_values.size();
    }
}