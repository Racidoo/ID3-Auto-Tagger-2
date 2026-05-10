#pragma once

#include <memory>

#include "Sources/IArtistSource.hpp"
#include "Spotify/Artist.h"

class SpotifyArtistSource : public IArtistSource {
  public:
    explicit SpotifyArtistSource(std::shared_ptr<Spotify::Artist> _artist)
        : artist(std::move(_artist)) {}
    ~SpotifyArtistSource() = default;

    std::string get_id() const override;
    std::string get_title() override;
    std::vector<std::byte> get_cover() override;

    void set_title(const std::string &_title) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;

    std::shared_ptr<Spotify::Artist> get_artist() const;

  private:
    std::shared_ptr<Spotify::Artist> artist;
};
