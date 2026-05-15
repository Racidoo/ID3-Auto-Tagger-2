#pragma once

#include <memory>

#include "IMediaEntity.hpp"

class IArtistSource;

namespace Discogs {
class Artist;
} // namespace Discogs

namespace Spotify {
class Artist;
} // namespace Spotify

class IArtist : public IMediaEntity {
  public:
    explicit IArtist(std::shared_ptr<IArtistSource> _source)
        : source(std::move(_source)) {}
    ~IArtist() = default;

    const std::string &get_id() const override;
    const std::string &get_name() const override;
    const std::vector<std::byte> &get_image() override;

    void ensureLoaded(class IMediaService &_service) override;

    static std::shared_ptr<IArtist>
    fromSpotify(std::shared_ptr<Spotify::Artist> _artist);
    static std::shared_ptr<IArtist>
    fromDiscogs(std::shared_ptr<Discogs::Artist> _artist);

  private:
    std::shared_ptr<IArtistSource> source;
};
