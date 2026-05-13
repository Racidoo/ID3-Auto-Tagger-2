#pragma once

#include <memory>

#include "Sources/IAlbumSource.hpp"

namespace Spotify {
class Album;
} // namespace Spotify

class ITrack;

class SpotifyAlbumSource : public IAlbumSource {
  public:
    explicit SpotifyAlbumSource(std::shared_ptr<Spotify::Album> _album)
        : album(std::move(_album)) {}
    ~SpotifyAlbumSource() = default;

    std::string get_id() const override;
    std::string get_title() override;
    std::vector<std::byte> get_cover() override;
    std::string get_artist() override;
    std::string get_copyright() override;
    std::string get_genre() override;
    std::string get_year() override;
    std::string get_label() override;

    std::string get_type() const override;
    std::vector<std::byte> get_artistCover() const override;
    std::vector<std::shared_ptr<ITrack>> get_tracklist() const override;
    bool isMetaDataComplete() const override;

    void set_title(const std::string &_title) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;
    void set_artist(const std::string &_artist) override;
    void set_copyright(const std::string &_copyright) override;
    void set_genre(const std::string &_genre) override;
    void set_year(const std::string &_year) override;
    void set_label(const std::string &_label) override;

    std::shared_ptr<Spotify::Album> get_album() const;

  private:
    std::shared_ptr<Spotify::Album> album;
};
