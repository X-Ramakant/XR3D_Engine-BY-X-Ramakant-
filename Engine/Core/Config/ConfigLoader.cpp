/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements ConfigLoader.

*/

#include "ConfigLoader.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace XR3D::Core::Config
{
    namespace
    {
        std::string Trim(const std::string& text)
        {
            size_t start = 0;
            size_t end = text.size();

            while (start < end && std::isspace(static_cast<unsigned char>(text[start])))
            {
                ++start;
            }
            while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])))
            {
                --end;
            }

            return text.substr(start, end - start);
        }

        bool IsCommentOrBlank(const std::string& line)
        {
            if (line.empty())
            {
                return true;
            }
            return line.rfind('#', 0) == 0 || line.rfind("//", 0) == 0;
        }

        bool LooksLikeInteger(const std::string& text)
        {
            if (text.empty())
            {
                return false;
            }

            size_t index = (text[0] == '-' || text[0] == '+') ? 1 : 0;
            if (index >= text.size())
            {
                return false;
            }

            for (; index < text.size(); ++index)
            {
                if (!std::isdigit(static_cast<unsigned char>(text[index])))
                {
                    return false;
                }
            }
            return true;
        }

        bool LooksLikeDouble(const std::string& text)
        {
            if (text.empty())
            {
                return false;
            }

            bool sawDigit = false;
            bool sawDot = false;
            size_t index = (text[0] == '-' || text[0] == '+') ? 1 : 0;

            for (; index < text.size(); ++index)
            {
                char character = text[index];
                if (std::isdigit(static_cast<unsigned char>(character)))
                {
                    sawDigit = true;
                }
                else if (character == '.' && !sawDot)
                {
                    sawDot = true;
                }
                else
                {
                    return false;
                }
            }

            return sawDigit && sawDot;
        }

        std::string StripQuotes(const std::string& text)
        {
            if (text.size() >= 2 && text.front() == '"' && text.back() == '"')
            {
                return text.substr(1, text.size() - 2);
            }
            return text;
        }
    }

    ConfigValue ConfigLoader::ParseValue(const std::string& rawValue)
    {
        const std::string value = Trim(rawValue);

        if (value == "true")
        {
            return ConfigValue{ true };
        }
        if (value == "false")
        {
            return ConfigValue{ false };
        }
        if (LooksLikeInteger(value))
        {
            return ConfigValue{ static_cast<int64_t>(std::stoll(value)) };
        }
        if (LooksLikeDouble(value))
        {
            return ConfigValue{ std::stod(value) };
        }

        return ConfigValue{ StripQuotes(value) };
    }

    std::string ConfigLoader::SerializeValue(const ConfigValue& value)
    {
        if (const bool* asBool = std::get_if<bool>(&value))
        {
            return *asBool ? "true" : "false";
        }
        if (const int64_t* asInt = std::get_if<int64_t>(&value))
        {
            return std::to_string(*asInt);
        }
        if (const double* asDouble = std::get_if<double>(&value))
        {
            return std::to_string(*asDouble);
        }
        if (const std::string* asString = std::get_if<std::string>(&value))
        {
            return "\"" + *asString + "\"";
        }

        return "";
    }

    bool ConfigLoader::LoadFromFile(const std::string& filePath, ConfigRegistry& outRegistry)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            return false;
        }

        std::string line;
        while (std::getline(file, line))
        {
            const std::string trimmed = Trim(line);
            if (IsCommentOrBlank(trimmed))
            {
                continue;
            }

            const size_t equalsPos = trimmed.find('=');
            if (equalsPos == std::string::npos)
            {
                continue; // Malformed line - skip, not fatal.
            }

            const std::string key = Trim(trimmed.substr(0, equalsPos));
            const std::string rawValue = trimmed.substr(equalsPos + 1);

            if (key.empty())
            {
                continue;
            }

            outRegistry.Set(key, ParseValue(rawValue));
        }

        return true;
    }

    bool ConfigLoader::SaveToFile(const std::string& filePath, const ConfigRegistry& registry)
    {
        std::ofstream file(filePath, std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            return false;
        }

        for (const std::string& key : registry.GetAllKeys())
        {
            auto value = registry.Get(key);
            if (value.has_value())
            {
                file << key << " = " << SerializeValue(value.value()) << '\n';
            }
        }

        return true;
    }
}