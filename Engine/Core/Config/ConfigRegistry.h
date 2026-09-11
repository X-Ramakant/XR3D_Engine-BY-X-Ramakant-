/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares ConfigRegistry, a thread-safe key-value store for config
* values.

*/

#pragma once

#include "ConfigTypes.h"

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace XR3D::Core::Config
{
    class ConfigRegistry
    {
    public:
        void Set(const std::string& key, ConfigValue value);
        bool Has(const std::string& key) const;
        std::optional<ConfigValue> Get(const std::string& key) const;
        void Remove(const std::string& key);
        void Clear();

        std::vector<std::string> GetAllKeys() const;
        size_t GetCount() const;

    private:
        mutable std::mutex m_mutex;
        std::unordered_map<std::string, ConfigValue> m_values;
    };
}