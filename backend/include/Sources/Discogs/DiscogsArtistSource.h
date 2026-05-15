#pragma once

#include <memory>

#include "Discogs/Artist.h"
#include "Sources/IArtistSource.hpp"

class DiscogsArtistSource : public IArtistSource {
  public:
    explicit DiscogsArtistSource(std::shared_ptr<Discogs::Artist> _artist)
        : artist(_artist) {}
    ~DiscogsArtistSource() = default;

    const std::string &get_id() const override;
    const std::string &get_name() override;
    const std::vector<std::byte> &get_image() override;
    IMediaEntity::State get_state() const override;

    void set_name(const std::string &_name) override;
    void set_image(const std::vector<std::byte> &_imageData) override;
    void set_state(IMediaEntity::State _state) override;

    std::shared_ptr<Discogs::Artist> get_artist() const;

    void ensureLoaded(class IMediaService &_service) override;

  private:
    std::shared_ptr<Discogs::Artist> artist;
};
