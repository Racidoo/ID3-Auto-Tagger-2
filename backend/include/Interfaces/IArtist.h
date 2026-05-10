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

    std::string get_id() const override;
    std::string get_title() override;
    std::vector<std::byte> get_cover() override;

    void set_title(const std::string &_title) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;

    static std::shared_ptr<IArtist>
    fromSpotify(std::shared_ptr<Spotify::Artist> _track);
    static std::shared_ptr<IArtist>
    fromDiscogs(std::shared_ptr<Discogs::Artist> _track);

  private:
    std::shared_ptr<IArtistSource> source;
};
