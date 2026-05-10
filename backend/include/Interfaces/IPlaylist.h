#pragma once

#include <memory>

#include "IMediaEntity.hpp"

class IPlaylistSource;

namespace Spotify {
class Playlist;
} // namespace Spotify

class IPlaylist : public IMediaEntity {
  public:
    explicit IPlaylist(std::shared_ptr<IPlaylistSource> _source)
        : source(std::move(_source)) {}
    ~IPlaylist() = default;

    std::string get_id() const override;
    std::string get_title() override;
    std::vector<std::byte> get_cover() override;

    void set_title(const std::string &_title) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;

    static std::shared_ptr<IPlaylist>
    fromSpotify(std::shared_ptr<Spotify::Playlist> _track);

  private:
    std::shared_ptr<IPlaylistSource> source;
};
