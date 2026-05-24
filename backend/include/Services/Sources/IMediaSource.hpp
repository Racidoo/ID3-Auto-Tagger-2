#pragma once

#include <memory>
#include <optional>

#include "Interfaces/ISearchResult.hpp"
#include "Interfaces/ITrack.h"

class IMediaSource {
  public:
    virtual ~IMediaSource() = default;

    struct SearchQuery {
        std::optional<std::string> title;
        std::optional<std::string> album;
        std::optional<std::string> albumArtist;
        std::optional<std::string> artist;
        std::optional<std::string> filename;
        std::optional<std::size_t> durationSeconds;

        SearchQuery(std::shared_ptr<ITrack> _track) {
            title = _track->get_name();
            album = _track->get_albumName();
            albumArtist = _track->get_albumArtist();
            artist = _track->get_artist();
            filename = _track->get_id();
            durationSeconds = _track->get_length();
        }
    };

    virtual IMediaService::MediaSourceId get_id() const = 0;

    virtual ISearchResult search(const std::string &_query,
                                 const SearchOptions &_options) = 0;
    virtual ISearchResult search(std::shared_ptr<ITrack> _track,
                                 const SearchOptions &_options) = 0;
    virtual bool supports(const std::string &_input) const = 0;
    virtual ISearchResult resolve(const std::string &_input) = 0;

    virtual std::shared_ptr<IAlbum> getAlbum(const std::string &_id) = 0;
    virtual std::shared_ptr<IArtist> getArtist(const std::string &_id) = 0;
    virtual std::shared_ptr<ITrack> getTrack(const std::string &_id) = 0;
};