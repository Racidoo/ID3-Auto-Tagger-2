project(SpotifyAPI_Frontend)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCES
    src/App.cpp
    src/main.cpp
    src/MainFrame.cpp
    src/TrackLabel.cpp
    src/TrackWindow.cpp
    src/ScrollText.cpp
)

# Find wxWidgets
# set(CMAKE_PREFIX_PATH /usr/include/wx-3.0/wx/)
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

find_package(wxWidgets REQUIRED)
message(STATUS "wxWidgets Include Dirs: ${wxWidgets_INCLUDE_DIRS}")


# Create the GUI executable
add_executable(frontend ${SOURCES})

# Include directories
target_include_directories(frontend PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/backend/include)

# Link against backend and wxWidgets
target_link_libraries(frontend PRIVATE backend ${wxWidgets_LIBRARIES})

# set(CMAKE_MODULE_PATH "/usr/lib/cmake/wxWidgets/")

# find_package(wxWidgets REQUIRED core base net adv xrc html xml)
# # include(${wxWidgets_USE_FILE})
# target_link_libraries(frontend ${wxWidgets_LIBRARIES})
