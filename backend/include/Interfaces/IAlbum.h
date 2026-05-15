#pragma once

#include <memory>

#include "IMediaEntity.hpp"

class IAlbumSource;
class IMediaService;

namespace Discogs {
class Release;
} // namespace Discogs

namespace Spotify {
class Album;
} // namespace Spotify

class ITrack;

class IAlbum : public IMediaEntity {
  public:
    explicit IAlbum(std::shared_ptr<IAlbumSource> _source)
        : source(std::move(_source)) {}
    ~IAlbum() = default;

    const std::string &get_id() const override;
    const std::string &get_name() const override;
    const std::vector<std::byte> &get_image() override;
    std::string get_type() const;
    std::string get_artist() const;
    std::vector<std::byte> get_artistCover() const;
    std::string get_year() const;
    std::vector<std::shared_ptr<ITrack>> get_tracklist() const;

    std::shared_ptr<Discogs::Release> get_discogsRelease() const;

    static std::shared_ptr<IAlbum>
    fromSpotify(std::shared_ptr<Spotify::Album> _album);
    static std::shared_ptr<IAlbum>
    fromDiscogs(std::shared_ptr<Discogs::Release> _release);

    void ensureLoaded(class IMediaService &_service) override;

  private:
    std::shared_ptr<IAlbumSource> source;
};
