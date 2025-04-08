# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(jarvisCPP)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# All .cpp and .c files in the workspace
file(GLOB_RECURSE SOURCES_CPP "${CMAKE_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SOURCES_C "${CMAKE_SOURCE_DIR}/*.c")

# Combine .cpp and .c files into a single list
set(SOURCES ${SOURCES_CPP} ${SOURCES_C})

# Exclude files in the landmina and build folders
list(FILTER SOURCES EXCLUDE REGEX ".*/landmina/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*/build/.*")

# Add the executable
add_executable(output ${SOURCES})

# Set the output directory for the executable
set_target_properties(output PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/output"
)