#pragma once

#include <memory>

#include "Sources/IArtistSource.hpp"
#include "Spotify/Artist.h"

class SpotifyArtistSource : public IArtistSource {
  public:
    explicit SpotifyArtistSource(std::shared_ptr<Spotify::Artist> _artist)
        : artist(std::move(_artist)) {}
    ~SpotifyArtistSource() = default;

    const std::string &get_id() const override;
    const std::string &get_name() override;
    const std::vector<std::byte> &get_image() override;
    IMediaEntity::State get_state() const override;

    void set_name(const std::string &_name) override;
    void set_image(const std::vector<std::byte> &_imageData) override;
    void set_state(IMediaEntity::State _state) override;
    std::shared_ptr<Spotify::Artist> get_artist() const;

    void ensureLoaded(class IMediaService &_service) override;

  private:
    std::shared_ptr<Spotify::Artist> artist;
};
