#pragma once

#include "Interfaces/IPlaylist.h"
#include "Interfaces/IRemoteImageProvider.h"

namespace Spotify {
class User;
class Playlist : public IPlaylist {
  public:
    Playlist(const std::string &_id, const std::string &_name, State _state,
             const std::string &_imageURL, std::shared_ptr<User> _creator,
             IMediaService *_mediaService);
    ~Playlist() = default;

    const std::string &get_name() const override;
    std::vector<std::byte> get_image() override;
    inline std::shared_ptr<User> get_creator() const { return creator; }

  private:
    std::string name;
    IRemoteImageProvider imageProvider;
    std::shared_ptr<User> creator;
};

} // namespace Spotify
