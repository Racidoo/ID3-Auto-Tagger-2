project(SpotifyAPI_Frontend)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCES
    src/AlbumLabel.cpp
    src/App.cpp
    src/ArtistLabel.cpp
    src/CircleProgressBar.cpp
    src/IconProvider.cpp
    src/LabeledTextCtrl.cpp
    src/LocalTrackWindow.cpp
    src/main.cpp
    src/MainFrame.cpp
    src/MediaLabel.cpp
    src/MultiInputDialog.cpp
    include/MediaWindow.hpp
    src/PlaylistLabel.cpp
    src/ScrollText.cpp
    src/SettingsWindow.cpp
    src/SpotifyWindow.cpp
    src/TrackEditWindow.cpp
    src/TrackLabel.cpp
    src/TrackWindow.cpp
    src/VideoLabel.cpp
    src/YouTubeWindow.cpp
)

# Find wxWidgets
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

if (ENABLE_ASAN)
    target_compile_options(ID3-Auto-Tagger-2 PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(ID3-Auto-Tagger-2 PRIVATE -fsanitize=address)
endif()

if (ENABLE_TSAN)
    target_compile_options(ID3-Auto-Tagger-2 PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(ID3-Auto-Tagger-2 PRIVATE -fsanitize=thread)
endif()