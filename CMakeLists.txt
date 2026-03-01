cmake_minimum_required(VERSION 3.10)
project(NES VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# # Optimization flags for performance
# if(NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE Release)
# endif()

add_compile_options(-O3 -g -fno-omit-frame-pointer)

# Include directories
include_directories(include)

# Find Raylib
find_package(raylib REQUIRED)

# Find all source files in src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the executable
add_executable(NES ${SOURCES})

# Link Raylib
target_link_libraries(NES raylib)
