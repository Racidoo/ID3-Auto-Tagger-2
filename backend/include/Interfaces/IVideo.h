#pragma once

#include <memory>

#include "MediaEntityBase.h"

class IVideo : public MediaEntityBase {
  public:
    explicit IVideo(const std::string &_id, State _state,
                    IMediaService *_mediaService)
        : MediaEntityBase(_id, _state, _mediaService) {}
    ~IVideo() = default;

    virtual std::size_t get_length() const = 0;
    virtual const std::string &get_channelTitle() const = 0;
    virtual const std::string &get_uploadDate() const = 0;
    virtual bool get_licensed() const = 0;
};