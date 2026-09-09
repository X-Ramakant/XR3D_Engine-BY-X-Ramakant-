# XR3D_Integration target
# Access: Engine/ source + Integration/ (this folder, including Scenarios/).
# NOT allowed: Tests/.

file(GLOB_RECURSE XR3D_INTEGRATION_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(XR3D_Integration ${XR3D_ENGINE_SOURCES} ${XR3D_ENGINE_HEADERS} ${XR3D_INTEGRATION_SOURCES})

target_include_directories(XR3D_Integration PRIVATE ${XR3D_ENGINE_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}")