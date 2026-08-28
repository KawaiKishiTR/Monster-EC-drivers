cmake_minimum_required(VERSION 3.20)
project(clevo_controller VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Shared Library (.so)
add_library(clevo_controller SHARED
    src/clevo_controller.cpp
    src/clevo_fan.cpp
    include/clevo_controller.h
    include/clevo_fan.hpp
    include/clevo_hw_defs.h
)

target_include_directories(clevo_controller PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# CLI Test Binary
add_executable(clevo_cli
    src/main.cpp
)

target_include_directories(clevo_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(clevo_cli PRIVATE clevo_controller)