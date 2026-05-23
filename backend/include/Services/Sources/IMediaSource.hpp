#pragma once

#include "Interfaces/ISearchResult.hpp"

class IMediaSource {
  public:
    virtual ~IMediaSource() = default;

    virtual IMediaService::MediaSourceId get_id() const = 0;

    virtual ISearchResult search(const std::string &_query,
                                 const SearchOptions &_options) = 0;
    virtual bool supports(const std::string &_input) const = 0;
    virtual ISearchResult resolve(const std::string &_input) = 0;

    virtual std::shared_ptr<IAlbum> getAlbum(const std::string &_id) = 0;
    virtual std::shared_ptr<IArtist> getArtist(const std::string &_id) = 0;
    virtual std::shared_ptr<ITrack> getTrack(const std::string &_id) = 0;
};