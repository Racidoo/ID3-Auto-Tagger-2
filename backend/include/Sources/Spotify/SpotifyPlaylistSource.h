#pragma once

#include <memory>

#include "Sources/IPlaylistSource.hpp"
#include "Spotify/Playlist.h"

class SpotifyPlaylistSource : public IPlaylistSource {
  public:
    explicit SpotifyPlaylistSource(std::shared_ptr<Spotify::Playlist> _playlist)
        : playlist(std::move(_playlist)) {}
    ~SpotifyPlaylistSource() = default;

    std::string get_id() const override;
    std::string get_title() override;
    std::vector<std::byte> get_cover() override;

    void set_title(const std::string &_title) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;

    std::shared_ptr<Spotify::Playlist> get_playlist() const;

  private:
    std::shared_ptr<Spotify::Playlist> playlist;
};
