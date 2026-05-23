#pragma once

#include <memory>

#include "MediaEntityBase.h"

class IArtist : public MediaEntityBase {
  public:
    explicit IArtist(const std::string &_id, State _state,
                     IMediaService *_mediaService)
        : MediaEntityBase(_id, _state, _mediaService) {}
    ~IArtist() = default;
};
