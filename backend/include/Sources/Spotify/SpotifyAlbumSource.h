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
        : album(_album) {}

    ~SpotifyAlbumSource() = default;

    const std::string &get_id() const override;
    const std::string &get_name() override;
    const std::vector<std::byte> &get_image() override;

    std::string get_artist() override;
    std::string get_copyright() override;
    std::string get_genre() override;
    std::string get_year() override;
    std::string get_label() override;
    IMediaEntity::State get_state() const override;
    std::string get_type() const override;
    std::vector<std::byte> get_artistCover() override;
    std::vector<std::shared_ptr<ITrack>> get_tracklist() const override;

    void set_name(const std::string &_name) override;
    void set_image(const std::vector<std::byte> &_imageData) override;
    void set_artist(const std::string &_artist) override;
    void set_copyright(const std::string &_copyright) override;
    void set_genre(const std::string &_genre) override;
    void set_year(const std::string &_year) override;
    void set_label(const std::string &_label) override;
    void set_state(IMediaEntity::State _state) override;

    void ensureLoaded(class IMediaService &_service) override;

  private:
    std::shared_ptr<Spotify::Album> album;
};
