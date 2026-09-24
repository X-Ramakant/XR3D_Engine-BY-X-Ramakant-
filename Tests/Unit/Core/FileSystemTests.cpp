#include "TestFramework.h"
#include "Core\FileSystem\FileSystemManager.h"
#include "Core\FileSystem\FileSystemPath.h"

#include <chrono>
#include <thread>

using namespace XR3D::Core::FileSystem;

namespace
{
    const std::string kTestDir = "fs_unit_test_data";
}

XR3D_TEST(FileSystem_Manager_InitializeAndShutdown)
{
    FileSystemManager::Get().Initialize();
    FileSystemManager::Get().Initialize(); // second call must be a safe no-op
    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_Path_ManipulationFunctions)
{
    XR3D_CHECK_EQ(FileSystemPath::GetExtension("model.fbx"), std::string(".fbx"));
    XR3D_CHECK_EQ(FileSystemPath::GetFileName("a/b/c.txt"), std::string("c.txt"));
    XR3D_CHECK_EQ(FileSystemPath::GetFileNameWithoutExtension("a/b/c.txt"), std::string("c"));
    XR3D_CHECK_EQ(FileSystemPath::Join("a/b", "c.txt"), std::string("a/b/c.txt"));
    return true;
}

XR3D_TEST(FileSystem_WriteAndReadText_RoundTrip)
{
    FileSystemManager::Get().Initialize();
    FileSystemManager::Get().CreateDirectory(kTestDir);

    const std::string path = kTestDir + "/round_trip.txt";
    FileError writeErr = FileSystemManager::Get().WriteAllText(path, "hello xr3d");
    XR3D_CHECK_EQ(static_cast<int>(writeErr), static_cast<int>(FileError::None));

    auto result = FileSystemManager::Get().ReadAllText(path);
    XR3D_CHECK(result.success);
    XR3D_CHECK_EQ(result.value, std::string("hello xr3d"));

    FileSystemManager::Get().DeleteFile(path);
    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_ReadMissingFile_ReturnsGracefulNotFound)
{
    FileSystemManager::Get().Initialize();

    auto result = FileSystemManager::Get().ReadAllText(kTestDir + "/does_not_exist.txt");
    XR3D_CHECK(!result.success);
    XR3D_CHECK_EQ(static_cast<int>(result.error), static_cast<int>(FileError::NotFound));
    XR3D_CHECK(result.value.empty());

    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_WriteAndReadBytes_RoundTrip)
{
    FileSystemManager::Get().Initialize();
    FileSystemManager::Get().CreateDirectory(kTestDir);

    const std::string path = kTestDir + "/round_trip.bin";
    std::vector<uint8_t> bytes = { 1, 2, 3, 255, 0, 128 };

    FileError writeErr = FileSystemManager::Get().WriteAllBytes(path, bytes);
    XR3D_CHECK_EQ(static_cast<int>(writeErr), static_cast<int>(FileError::None));

    auto result = FileSystemManager::Get().ReadAllBytes(path);
    XR3D_CHECK(result.success);
    XR3D_CHECK(result.value == bytes);

    FileSystemManager::Get().DeleteFile(path);
    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_Exists_And_IsDirectory)
{
    FileSystemManager::Get().Initialize();
    FileSystemManager::Get().CreateDirectory(kTestDir);

    const std::string path = kTestDir + "/exists_test.txt";
    FileSystemManager::Get().WriteAllText(path, "x");

    XR3D_CHECK(FileSystemManager::Get().Exists(path));
    XR3D_CHECK(!FileSystemManager::Get().Exists(kTestDir + "/never_created.txt"));
    XR3D_CHECK(FileSystemManager::Get().IsDirectory(kTestDir));
    XR3D_CHECK(!FileSystemManager::Get().IsDirectory(path));

    FileSystemManager::Get().DeleteFile(path);
    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_CreateDirectory_Nested)
{
    FileSystemManager::Get().Initialize();

    const std::string nested = kTestDir + "/nested/deep/dir";
    FileError err = FileSystemManager::Get().CreateDirectory(nested);
    XR3D_CHECK_EQ(static_cast<int>(err), static_cast<int>(FileError::None));
    XR3D_CHECK(FileSystemManager::Get().IsDirectory(nested));

    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_DeleteFile_MissingFile_ReturnsGracefulNotFound)
{
    FileSystemManager::Get().Initialize();

    FileError err = FileSystemManager::Get().DeleteFile(kTestDir + "/never_existed.bin");
    XR3D_CHECK_EQ(static_cast<int>(err), static_cast<int>(FileError::NotFound));

    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_ListDirectory_ContainsCreatedFile)
{
    FileSystemManager::Get().Initialize();
    FileSystemManager::Get().CreateDirectory(kTestDir + "/listing");

    const std::string path = kTestDir + "/listing/marker.txt";
    FileSystemManager::Get().WriteAllText(path, "marker");

    auto listing = FileSystemManager::Get().ListDirectory(kTestDir + "/listing");
    XR3D_CHECK(listing.success);

    bool found = false;
    for (auto& name : listing.value)
    {
        if (name == "marker.txt")
        {
            found = true;
        }
    }
    XR3D_CHECK(found);

    FileSystemManager::Get().DeleteFile(path);
    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_VirtualMount_ResolvesCorrectly)
{
    FileSystemManager::Get().Initialize();
    FileSystemManager::Get().CreateDirectory(kTestDir + "/mount_target");

    FileSystemManager::Get().RegisterMount("testmount", kTestDir + "/mount_target");
    XR3D_CHECK(FileSystemManager::Get().HasMount("testmount"));

    FileError writeErr = FileSystemManager::Get().WriteAllText("testmount://file.txt", "mounted");
    XR3D_CHECK_EQ(static_cast<int>(writeErr), static_cast<int>(FileError::None));

    auto readResult = FileSystemManager::Get().ReadAllText("testmount://file.txt");
    XR3D_CHECK(readResult.success);
    XR3D_CHECK_EQ(readResult.value, std::string("mounted"));

    FileSystemManager::Get().GetRegistry().RemoveMount("testmount");
    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_UnknownMountAlias_PassesThroughGracefully)
{
    FileSystemManager::Get().Initialize();

    auto result = FileSystemManager::Get().ReadAllText("unknownalias://foo.txt");
    XR3D_CHECK(!result.success);
    XR3D_CHECK_EQ(static_cast<int>(result.error), static_cast<int>(FileError::NotFound));

    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_Watcher_DetectsChangeAndFires)
{
    FileSystemManager::Get().Initialize();
    FileSystemManager::Get().CreateDirectory(kTestDir);

    const std::string path = kTestDir + "/watched.txt";
    FileSystemManager::Get().WriteAllText(path, "initial");

    bool fired = false;
    WatchHandle handle = FileSystemManager::Get().Watch(path, [&]() { fired = true; });
    XR3D_CHECK(handle.IsValid());

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    FileSystemManager::Get().WriteAllText(path, "changed");
    FileSystemManager::Get().PollWatches();

    XR3D_CHECK(fired);

    FileSystemManager::Get().Unwatch(handle);
    XR3D_CHECK(!FileSystemManager::Get().GetWatcher().IsValid(handle));

    FileSystemManager::Get().DeleteFile(path);
    FileSystemManager::Get().Shutdown();
    return true;
}

XR3D_TEST(FileSystem_Watcher_ReentrantCallback_DoesNotDeadlock)
{
    FileSystemManager::Get().Initialize();
    FileSystemManager::Get().CreateDirectory(kTestDir);

    const std::string path = kTestDir + "/reentrant_watch.txt";
    FileSystemManager::Get().WriteAllText(path, "initial");

    bool innerValid = false;
    FileSystemManager::Get().Watch(path, [&]() {
        WatchHandle inner = FileSystemManager::Get().Watch(path, []() {});
        innerValid = FileSystemManager::Get().GetWatcher().IsValid(inner);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    FileSystemManager::Get().WriteAllText(path, "changed");
    FileSystemManager::Get().PollWatches(); // must not hang - proves no deadlock

    XR3D_CHECK(innerValid);

    FileSystemManager::Get().DeleteFile(path);
    FileSystemManager::Get().Shutdown();
    return true;
}