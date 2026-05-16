#pragma once

#include "Interfaces/IArtist.h"

namespace Spotify {

class Artist : public IArtist {
  private:
  public:
    Artist(const std::string &_id, const std::string &_name, State _state,
           const std::string &_imageURL);
    ~Artist() = default;
};

} // namespace Spotify
