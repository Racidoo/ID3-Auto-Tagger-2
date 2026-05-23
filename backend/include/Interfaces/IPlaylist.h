#pragma once

#include <memory>

#include "MediaEntityBase.h"

class IPlaylist : public MediaEntityBase {
  public:
    explicit IPlaylist(const std::string &_id, State _state,
                       IMediaService *_mediaService)
        : MediaEntityBase(_id, _state, _mediaService) {}
    ~IPlaylist() = default;
};
