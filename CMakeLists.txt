cmake_minimum_required(VERSION 3.10)
project(NES VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Find all source files in src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the executable
add_executable(NES ${SOURCES})
