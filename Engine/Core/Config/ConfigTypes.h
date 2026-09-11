/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Defines shared types used across the Config system: the ConfigValue
* variant type and its associated ConfigValueType enum.

*/

#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace XR3D::Core::Config
{
    enum class ConfigValueType : uint8_t
    {
        Bool = 0,
        Int,
        Double,
        String,
        Count
    };

    using ConfigValue = std::variant<bool, int64_t, double, std::string>;

    constexpr const char* ConfigValueTypeToString(ConfigValueType type)
    {
        switch (type)
        {
        case ConfigValueType::Bool:   return "Bool";
        case ConfigValueType::Int:    return "Int";
        case ConfigValueType::Double: return "Double";
        case ConfigValueType::String: return "String";
        default:                      return "Unknown";
        }
    }

    // TODO (Future): add Vector3/Color config value types once the
    // Engine has shared math types to depend on.
}