#pragma once

#include <memory>

#include "Sources/IPlaylistSource.hpp"
#include "Spotify/Playlist.h"

class SpotifyPlaylistSource : public IPlaylistSource {
  public:
    explicit SpotifyPlaylistSource(std::shared_ptr<Spotify::Playlist> _playlist)
        : playlist(std::move(_playlist)) {}
    ~SpotifyPlaylistSource() = default;

    const std::string &get_id() const override;
    const std::string &get_name() override;
    const std::vector<std::byte> &get_image() override;
    IMediaEntity::State get_state() const override;

    void set_name(const std::string &_name) override;
    void set_image(const std::vector<std::byte> &_imageData) override;
    void set_state(IMediaEntity::State _state) override;
    std::shared_ptr<Spotify::Playlist> get_playlist() const;

    void ensureLoaded(class IMediaService &_service) override;

  private:
    std::shared_ptr<Spotify::Playlist> playlist;
};
