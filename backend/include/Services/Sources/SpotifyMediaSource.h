#pragma once

#include <memory>
#include <regex>

#include "Services/Sources/IMediaSource.hpp"
#include "Spotify/SpotifyAPI.h"

class SpotifyMediaSource : public IMediaSource {
  public:
    SpotifyMediaSource(Spotify::SpotifyAPI &_spotify);
    ~SpotifyMediaSource() = default;

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
    Spotify::SpotifyAPI &spotify;
    std::regex pattern = std::regex(
        R"(https:\/\/open\.spotify\.com\/(?:intl-[a-z]{2}\/)?(playlist|album|track)\/([\w]+))");
};
