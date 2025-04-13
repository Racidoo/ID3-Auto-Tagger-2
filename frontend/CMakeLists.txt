project(SpotifyAPI_Frontend)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCES
    src/AlbumLabel.cpp
    src/App.cpp
    src/ArtistLabel.cpp
    src/CircleProgressBar.cpp
    src/LabeledTextCtrl.cpp
    src/IconProvider.cpp
    src/main.cpp
    src/MainFrame.cpp
    src/MediaLabel.cpp
    include/MediaWindow.hpp
    src/PlaylistLabel.cpp
    src/ScrollText.cpp
    src/SettingsWindow.cpp
    src/SpotifyWindow.cpp
    src/TrackEditWindow.cpp
    src/TrackLabel.cpp
    src/TrackWindow.cpp
)

# Find wxWidgets
# set(CMAKE_PREFIX_PATH /usr/include/wx-3.0/wx/)
find_package(wxWidgets REQUIRED COMPONENTS core base adv)
include(${wxWidgets_USE_FILE})

find_package(wxWidgets REQUIRED)
message(STATUS "wxWidgets Include Dirs: ${wxWidgets_INCLUDE_DIRS}")


# Create the GUI executable
add_executable(ID3-Auto-Tagger-2 ${SOURCES})

# Include directories
target_include_directories(ID3-Auto-Tagger-2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/backend/include)

# Link against backend and wxWidgets
target_link_libraries(ID3-Auto-Tagger-2 PRIVATE backend ${wxWidgets_LIBRARIES})

# set(CMAKE_MODULE_PATH "/usr/lib/cmake/wxWidgets/")

# find_package(wxWidgets REQUIRED core base net adv xrc html xml)
# # include(${wxWidgets_USE_FILE})
# target_link_libraries(frontend ${wxWidgets_LIBRARIES})
