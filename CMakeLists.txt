cmake_minimum_required(VERSION 3.20) 
project(XR3D_Engine LANGUAGES CXX) 
 
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
 
# ========================================================== 
# This root file LINKS and MANAGES all 3 targets. 
# It does not build anything itself - each target is defined 
# inside its own CMakeLists.txt: 
#   Engine\CMakeLists.txt       -
#   Tests\CMakeLists.txt        -
#   Integration\CMakeLists.txt  -
# 
# Engine MUST be added first - it exports XR3D_ENGINE_SOURCES, 
# XR3D_ENGINE_HEADERS and XR3D_ENGINE_INCLUDE_DIR, which Tests 
# and Integration consume to build against the same Engine code. 
# ========================================================== 
 
add_subdirectory(Engine) 
add_subdirectory(Tests) 
add_subdirectory(Integration) 
