project(ID3-Auto-Tagger_Backend)

# Add source files
set(SOURCES
    # src/main.cpp
    lib/base64/base64.cpp
    src/Discogs/Artist.cpp
    src/Discogs/DiscogsAPI.cpp
    src/Discogs/Label.cpp
    src/Discogs/MasterRelease.cpp
    src/Discogs/Release.cpp
    src/Discogs/ReleaseTrack.cpp
    src/Downloader.cpp
    src/Interfaces/IAlbum.cpp
    src/Interfaces/IArtist.cpp
    src/Interfaces/ITrack.cpp
    src/Interfaces/IPlaylist.cpp
    src/Interfaces/IVideo.cpp
    src/LocalTrack.cpp
    src/LocalTrackService.cpp
    src/Query.cpp
    src/QueryObject.cpp
    src/Spotify/Album.cpp
    src/Spotify/Artist.cpp
    src/Spotify/Playlist.cpp
    src/Spotify/SpotifyAPI.cpp
    src/Spotify/Track.cpp
    src/Spotify/User.cpp
    src/Sources/Discogs/DiscogsArtistSource.cpp
    src/Sources/Discogs/DiscogsReleaseSource.cpp
    src/Sources/Discogs/DiscogsMasterReleaseSource.cpp
    src/Sources/Discogs/DiscogsTrackSource.cpp
    src/Sources/LocalTrackSource.cpp
    src/Sources/Spotify/SpotifyAlbumSource.cpp
    src/Sources/Spotify/SpotifyArtistSource.cpp
    src/Sources/Spotify/SpotifyPlaylistSource.cpp
    src/Sources/Spotify/SpotifyTrackSource.cpp
    src/Sources/YouTube/YouTubeVideoSource.cpp
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

if(ENABLE_ASAN)
    target_compile_options(backend PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(backend PRIVATE -fsanitize=address)
endif()

if(ENABLE_TSAN)
    target_compile_options(backend PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    target_link_options(backend PRIVATE -fsanitize=thread)
endif()
