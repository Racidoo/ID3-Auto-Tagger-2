#pragma once

#include <memory>

#include "MediaEntityBase.h"

class IArtist : public MediaEntityBase {
  public:
    explicit IArtist(const std::string &_id, const std::string &_name,
                     State _state, const std::string &_imageURL)
        : MediaEntityBase(_id, _name, _state, _imageURL) {}
    ~IArtist() = default;
};
