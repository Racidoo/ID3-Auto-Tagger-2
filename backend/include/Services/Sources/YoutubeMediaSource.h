#pragma once

#include <memory>
#include <regex>

#include "Services/Sources/IMediaSource.hpp"
#include "YouTube/YouTubeAPI.h"

class YoutubeMediaSource : public IMediaSource {
  public:
    YoutubeMediaSource(YouTube::YouTubeAPI &_youtube);
    ~YoutubeMediaSource() = default;

    IMediaService::MediaSourceId get_id() const override;

    ISearchResult search(const std::string &_query,
                         const SearchOptions &_options) override;
    ISearchResult search(std::shared_ptr<ITrack> _track,
                         const SearchOptions &_options) override;
    bool supports(const std::string &_input) const override;
    ISearchResult resolve(const std::string &_input) override;

    std::shared_ptr<IAlbum> getAlbum(const std::string &_id) override;
    std::shared_ptr<IArtist> getArtist(const std::string &_id) override;
    std::shared_ptr<ITrack> getTrack(const std::string &_id) override;

  private:
    YouTube::YouTubeAPI &youtube;
    std::regex pattern =
        std::regex(R"(youtu(?:\.be\/|be\.com\/watch\?v=)([A-Za-z0-9_-]{11}))");
};
