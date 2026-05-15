#pragma once

#include <string>

#include "Interfaces/MediaEntityBase.h"

namespace Discogs {

class Artist : public MediaEntityBase {
  public:
    Artist(int _id, const std::string &_name, State _state,
           const std::string &_imageUrl);
    ~Artist() = default;


    // void ensureLoaded(IMediaService& service) override;
  private:
    /* data */
};

} // namespace Discogs
