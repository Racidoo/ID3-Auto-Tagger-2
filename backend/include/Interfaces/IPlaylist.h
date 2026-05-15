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

    const std::string &get_id() const override;
    const std::string &get_name() const override;
    const std::vector<std::byte> &get_image() override;
   
    void ensureLoaded(class IMediaService &_service) override;

    static std::shared_ptr<IPlaylist>
    fromSpotify(std::shared_ptr<Spotify::Playlist> _track);

  private:
    std::shared_ptr<IPlaylistSource> source;
};
