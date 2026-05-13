#pragma once

#include <memory>

#include "IMediaEntity.hpp"

class IAlbumSource;

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

    std::string get_id() const override;
    std::string get_title() override;
    std::vector<std::byte> get_cover() override;
    std::string get_type() const;
    std::string get_artist() const;
    std::vector<std::byte> get_artistCover() const;
    std::string get_year() const;

    std::vector<std::shared_ptr<ITrack>> get_tracklist() const;
    bool isMetaDataComplete() const;

    void set_title(const std::string &_title) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;

    std::shared_ptr<Spotify::Album> get_spotifyAlbum() const;
    std::shared_ptr<Discogs::Release> get_discogsRelease() const;

    static std::shared_ptr<IAlbum>
    fromSpotify(std::shared_ptr<Spotify::Album> _album);
    static std::shared_ptr<IAlbum>
    fromDiscogs(std::shared_ptr<Discogs::Release> _release);

  private:
    std::shared_ptr<IAlbumSource> source;
};
