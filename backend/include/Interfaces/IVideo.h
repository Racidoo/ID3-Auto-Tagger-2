#pragma once

#include <memory>

#include "MediaEntityBase.h"

class IVideo : public MediaEntityBase {
  public:
    explicit IVideo(const std::string &_id, const std::string &_name,
                    State _state, const std::string &_imageURL,
                   IMediaService *_mediaService)
        : MediaEntityBase(_id, _name, _state, _imageURL, _mediaService) {}
    ~IVideo() = default;

    virtual std::size_t get_length() const = 0;

};