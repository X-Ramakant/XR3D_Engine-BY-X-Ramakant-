/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Minimal self-contained integration test framework (no third-party
* dependency). Deliberately a separate copy from Tests/Unit/TestFramework.h
* so the Integration target never depends on the Tests/ folder.

*/

#pragma once

#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace XR3D::Integration
{
    struct TestCase
    {
        std::string name;
        std::function<bool(std::string&)> fn;
    };

    inline std::vector<TestCase>& GetRegistry()
    {
        static std::vector<TestCase> registry;
        return registry;
    }

    struct TestRegistrar
    {
        TestRegistrar(const std::string& name, std::function<bool(std::string&)> fn)
        {
            GetRegistry().push_back({ name, fn });
        }
    };

    inline int RunAllTests()
    {
        int failed = 0;
        for (auto& test : GetRegistry())
        {
            std::string failMsg;
            bool passed = test.fn(failMsg);

            if (passed)
            {
                std::printf("[PASS] %s\n", test.name.c_str());
            }
            else
            {
                std::printf("[FAIL] %s - %s\n", test.name.c_str(), failMsg.c_str());
                ++failed;
            }
        }

        std::printf("\n%zu integration scenario(s) run, %d failed\n", GetRegistry().size(), failed);
        return failed == 0 ? 0 : 1;
    }
}

#define XR3D_SCENARIO(name) \
    static bool XR3D_SCENARIO_FN_##name([[maybe_unused]] std::string& XR3D_FailMsg); \
    static ::XR3D::Integration::TestRegistrar XR3D_SCENARIO_REG_##name(#name, XR3D_SCENARIO_FN_##name); \
    static bool XR3D_SCENARIO_FN_##name([[maybe_unused]] std::string& XR3D_FailMsg)

#define XR3D_CHECK(cond) \
    do { if (!(cond)) { XR3D_FailMsg = "CHECK failed: " #cond; return false; } } while (0)

#define XR3D_CHECK_EQ(a, b) \
    do { if (!((a) == (b))) { XR3D_FailMsg = "CHECK_EQ failed: " #a " == " #b; return false; } } while (0)

#define XR3D_CHECK_NULL(ptr) \
    do { if ((ptr) != nullptr) { XR3D_FailMsg = "CHECK_NULL failed: " #ptr " is not null"; return false; } } while (0)

#define XR3D_CHECK_NOT_NULL(ptr) \
    do { if ((ptr) == nullptr) { XR3D_FailMsg = "CHECK_NOT_NULL failed: " #ptr " is null"; return false; } } while (0)