# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Define the project name and version
project(ID3-Auto-Tagger-2 VERSION 0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Optional: enable sanitizers
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)

# propagate to subdirectories
set(ENABLE_ASAN ${ENABLE_ASAN} CACHE BOOL "Enable ASan")
set(ENABLE_TSAN ${ENABLE_TSAN} CACHE BOOL "Enable TSan")

set(CMAKE_BUILD_TYPE Debug)

# Add Backend and Frontend
add_subdirectory(backend)
add_subdirectory(frontend)