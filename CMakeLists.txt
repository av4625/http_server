cmake_minimum_required(VERSION 3.12)
project(http_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MACOSX_RPATH 1)

# Gtest and boost seem to have been built with different visibilities, this
# fixes warnings about boost but gives warnings about gtest
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

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

add_subdirectory(examples)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(third_party)
