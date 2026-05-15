#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Interfaces/MediaEntityBase.h"

namespace Spotify {

class Artist : public MediaEntityBase {
  private:
  public:
    Artist(const std::string &_id, const std::string &_name, State _state,
           const std::string &_imageURL);
    ~Artist() = default;

    // void loadAdditionalData(std::weak_ptr<IMediaService> _service) override;
};

} // namespace Spotify
