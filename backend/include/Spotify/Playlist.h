#pragma once

#include "Interfaces/IPlaylist.h"
#include "User.h"

namespace Spotify {
class Playlist : public IPlaylist {
  private:
    User creator;

  public:
    Playlist(const std::string &_id, const std::string &_name, State _state,
             const std::string &_imageURL, const User &_creator);
    ~Playlist() = default;

    inline User get_creator() const { return creator; }
    // void loadAdditionalData(std::weak_ptr<IMediaService> _service) override;
};

} // namespace Spotify
