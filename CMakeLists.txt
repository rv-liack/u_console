cmake_minimum_required(VERSION 3.24)
project(u_console VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SFML is now in /usr/local, so find_package will find it automatically
find_package(SFML 3.1 COMPONENTS Graphics REQUIRED)

# Library: u_console
add_library(u_console src/Console.cpp)

target_include_directories(u_console PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link SFML::Graphics publicly
target_link_libraries(u_console PUBLIC SFML::Graphics)

# Tests: u_console_tests
add_executable(u_console_tests tests/main.cpp)
target_link_libraries(u_console_tests PRIVATE u_console)
