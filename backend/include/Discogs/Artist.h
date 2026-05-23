#pragma once

#include "Interfaces/IArtist.h"

namespace Discogs {

class Artist : public IArtist {
  public:
    Artist(int _id, const std::string &_name, State _state,
           const std::string &_imageUrl, IMediaService *_mediaService);
    ~Artist() = default;
};

} // namespace Discogs
