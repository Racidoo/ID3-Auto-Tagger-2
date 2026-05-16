#pragma once

#include <memory>

#include "MediaEntityBase.h"

class IPlaylist : public MediaEntityBase {
  public:
    explicit IPlaylist(const std::string &_id, const std::string &_name,
                       State _state, const std::string &_imageURL)
        : MediaEntityBase(_id, _name, _state, _imageURL) {}
    ~IPlaylist() = default;


};
