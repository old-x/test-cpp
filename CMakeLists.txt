cmake_minimum_required(VERSION 3.10)

project("test-cpp")

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wpedantic -Wshadow")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/bin")
set(TEST_LIB_DIR "${PROJECT_SOURCE_DIR}/tests/lib")
set(STATIC_STD_GCC_FLAGS "-static-libstdc++" "-static-libgcc")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")
