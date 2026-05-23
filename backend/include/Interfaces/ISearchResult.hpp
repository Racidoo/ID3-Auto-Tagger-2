#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "Interfaces/IAlbum.h"
#include "Interfaces/IArtist.h"
#include "Interfaces/IPlaylist.h"
#include "Interfaces/ITrack.h"
#include "Interfaces/IVideo.h"
#include "Services/IMediaService.hpp"

struct ISearchResult {
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

struct SearchOptions {
    std::unordered_set<ISearchResult::SearchCategory> categories;

    std::size_t limit = 5;
    std::string market = "DE";

    std::unordered_set<IMediaService::MediaSourceId> sources;

    bool aggregate = true;
};
