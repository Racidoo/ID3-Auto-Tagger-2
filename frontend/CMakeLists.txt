project(SpotifyAPI_Frontend)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCES
    src/Core/App.cpp
    src/Core/IconProvider.cpp
    src/Core/MainFrame.cpp
    src/Components/CircleProgressBar.cpp
    src/Components/LabeledTextCtrl.cpp
    src/Components/MediaLabel.cpp
    src/Components/ScrollText.cpp
    src/Dialogs/MultiInputDialog.cpp
    src/Dialogs/TrackSelectionDialog.cpp
    src/Media/Album/AlbumDetailsPanel.cpp
    src/Media/Track/ComparisonTrackPanel.cpp
    src/Media/Track/LocalTrackPanel.cpp
    src/Media/Track/TrackPanel.cpp
    src/Media/Track/TrackModel.cpp
    src/Media/Track/TrackModelRow.cpp
    src/Media/Track/Renderers/DownloadStatusRenderer.cpp
    src/Windows/DiscogsWindow.cpp
    src/Windows/LocalTrackWindow.cpp
    src/Windows/SettingsWindow.cpp
    src/Windows/SpotifyWindow.cpp
    src/Windows/TrackEditWindow.cpp
    src/Windows/YouTubeWindow.cpp
    src/main.cpp
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

if(ENABLE_ASAN)
    target_compile_options(ID3-Auto-Tagger-2 PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(ID3-Auto-Tagger-2 PRIVATE -fsanitize=address)
endif()

if(ENABLE_TSAN)
    target_compile_options(ID3-Auto-Tagger-2 PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(ID3-Auto-Tagger-2 PRIVATE -fsanitize=thread)
endif()
