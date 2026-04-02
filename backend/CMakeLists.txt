project(SpotifyAPI_Backend)

# Add source files
set(SOURCES
    # src/main.cpp
    lib/base64/base64.cpp
    src/Downloader.cpp
    src/Query.cpp
    src/QueryObject.cpp
    src/Spotify/Album.cpp
    src/Spotify/Artist.cpp
    src/Spotify/Playlist.cpp
    src/Spotify/SpotifyAPI.cpp
    src/Spotify/Track.cpp
    src/Spotify/User.cpp
    src/YouTube/Video.cpp
    src/YouTube/YouTubeAPI.cpp
    # Add more source files here
)

# add_executable(backend ${SOURCES})

# Create a library from backend sources
add_library(backend STATIC ${SOURCES})

# Include directories
target_include_directories(backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find and link dependencies (e.g., cURL, JSON, Taglib)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)
# find_package(TAGLIB REQUIRED)

target_include_directories(backend PRIVATE ${TAGLIB_INCLUDE_DIRS})
target_link_libraries(backend PRIVATE CURL::libcurl nlohmann_json::nlohmann_json ${TAGLIB_LIBRARIES})
