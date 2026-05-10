#pragma once

#include <memory>
#include <vector>

#include "Interfaces/IAlbum.h"
#include "Interfaces/IArtist.h"
#include "Interfaces/IPlaylist.h"
#include "Interfaces/ITrack.h"
#include "Interfaces/IVideo.h"

class ISearchResult {
  public:
    ISearchResult() = default;
    ~ISearchResult() = default;

    enum class SearchCategory {
        Track,
        Album,
        Artist,
        Playlist,
        Video
        // Add more categories here if needed
    };

    std::vector<std::shared_ptr<ITrack>> tracks;
    std::vector<std::shared_ptr<IAlbum>> albums;
    std::vector<std::shared_ptr<IArtist>> artists;
    std::vector<std::shared_ptr<IPlaylist>> playlists;
    std::vector<std::shared_ptr<IVideo>> videos;
};
