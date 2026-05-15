#pragma once

#include "Interfaces/MediaEntityBase.h"
#include "User.h"

namespace Spotify {
class Playlist : public MediaEntityBase {
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
