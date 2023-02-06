cmake_minimum_required(VERSION 3.12)
project(http_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MACOSX_RPATH 1)

add_compile_options(-Wall -Wextra -pedantic -Werror)

if(COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fprofile-arcs -ftest-coverage")
endif()

# We default to Release build type if DCMAKE_BUILD_TYPE not provided
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(third_party)
