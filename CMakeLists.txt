# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Define the project name and version
project(ID3-Auto-Tagger-2 VERSION 0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

# Add Backend and Frontend
add_subdirectory(backend)
add_subdirectory(frontend)