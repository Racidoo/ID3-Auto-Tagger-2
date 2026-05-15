#pragma once

#include "Interfaces/MediaEntityBase.h"

namespace Spotify {
class User : public MediaEntityBase {
  public:
    User(const std::string &_id, const std::string &_name, State _state,
         const std::string &_imageURL);
    ~User() = default;

    // void loadAdditionalData(std::weak_ptr<IMediaService> _service) override;
};

} // namespace Spotify
