# ========================================================== 
# XR3D_Engine target ONLY 
# This file defines the Main Engine target and nothing else. 
# It does NOT create XR3D_UnitTests or XR3D_Integration - 
# those targets are each defined in their OWN CMakeLists.txt 
# (Tests\CMakeLists.txt and Integration\CMakeLists.txt). 
# This file only EXPOSES the Engine source list so those two 
# files can consume it - it does not manage or build them. 
# ========================================================== 
 
# Access: full Engine/ source tree + root Main.cpp only. 
# NOT allowed: Tests/, Integration/. 
 
file(GLOB_RECURSE XR3D_ENGINE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp") 
file(GLOB_RECURSE XR3D_ENGINE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
 
add_executable(XR3D_Engine ${XR3D_ENGINE_SOURCES} ${XR3D_ENGINE_HEADERS} "${CMAKE_SOURCE_DIR}/Main.cpp") 
 
target_include_directories(XR3D_Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}) 
 
# ---- Export for Tests\CMakeLists.txt and Integration\CMakeLists.txt ---- 
set(XR3D_ENGINE_SOURCES ${XR3D_ENGINE_SOURCES} PARENT_SCOPE) 
set(XR3D_ENGINE_HEADERS ${XR3D_ENGINE_HEADERS} PARENT_SCOPE) 
set(XR3D_ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE) 
