cmake_minimum_required(VERSION 3.10)

project("test-cpp")

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wpedantic -Wshadow")
set(TEST_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fsanitize=address -fsanitize=leak")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install")
set(STATIC_STD_GCC_FLAGS "-static-libstdc++" "-static-libgcc")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")
