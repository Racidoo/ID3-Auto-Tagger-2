#pragma once

#include <memory>

#include "MediaEntityBase.h"

class IVideo : public MediaEntityBase {
  public:
    explicit IVideo(const std::string &_id, const std::string &_name,
                    State _state, const std::string &_imageURL)
        : MediaEntityBase(_id, _name, _state, _imageURL) {}
    ~IVideo() = default;
};