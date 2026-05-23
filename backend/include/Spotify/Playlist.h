#pragma once

#include "Interfaces/IPlaylist.h"

namespace Spotify {
class User;
class Playlist : public IPlaylist {
  public:
    Playlist(const std::string &_id, const std::string &_name, State _state,
             const std::string &_imageURL, std::shared_ptr<User> _creator,
             IMediaService *_mediaService);
    ~Playlist() = default;

    inline std::shared_ptr<User> get_creator() const { return creator; }

  private:
    std::shared_ptr<User> creator;
};

} // namespace Spotify
