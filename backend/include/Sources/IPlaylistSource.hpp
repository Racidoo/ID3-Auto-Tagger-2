#pragma once

#include <string>
#include <vector>

#include "IMediaSource.hpp"

class IPlaylistSource : public IMediaSource {
  public:
    virtual ~IPlaylistSource() = default;
};
