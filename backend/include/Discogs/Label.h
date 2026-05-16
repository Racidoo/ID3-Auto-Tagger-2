#pragma once

#include "Interfaces/MediaEntityBase.h"

namespace Discogs {

class Label : public MediaEntityBase {
  public:
    Label(int _id, const std::string &_name, State _state,
          const std::string &_imageUrl);
    ~Label() = default;
};

} // namespace Discogs
