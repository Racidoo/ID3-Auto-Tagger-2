#pragma once

#include <memory>

#include "Discogs/Artist.h"
#include "Sources/IArtistSource.hpp"

class DiscogsArtistSource : public IArtistSource {
  public:
    explicit DiscogsArtistSource(std::shared_ptr<Discogs::Artist> _artist)
        : artist(std::move(_artist)) {}
    ~DiscogsArtistSource() = default;

    std::string get_id() const override;
    std::string get_title() override;
    std::vector<std::byte> get_cover() override;

    void set_title(const std::string &_title) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;

    std::shared_ptr<Discogs::Artist> get_artist() const;

  private:
    std::shared_ptr<Discogs::Artist> artist;
};
