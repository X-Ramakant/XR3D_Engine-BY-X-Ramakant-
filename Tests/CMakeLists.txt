# XR3D_UnitTests target
# Access: Engine/ source (engine files only, no third-party by default) + Tests/Unit/.
# NOT allowed: Integration/.

file(GLOB_RECURSE XR3D_UNITTEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Unit/*.cpp")

add_executable(XR3D_UnitTests ${XR3D_ENGINE_SOURCES} ${XR3D_ENGINE_HEADERS} ${XR3D_UNITTEST_SOURCES})

target_include_directories(XR3D_UnitTests PRIVATE ${XR3D_ENGINE_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/Unit")

# Third-party libraries are NOT linked here by default.
# If a specific test needs one, add target_link_libraries below with
# a comment explaining why it is required.