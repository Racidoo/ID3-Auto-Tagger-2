#pragma once

#include <string>
#include <vector>

#include "IMediaService.hpp"
#include "Interfaces/IMediaEntity.hpp"

class IMediaSource {
  public:
    ~IMediaSource() = default;

    virtual const std::string &get_id() const = 0;
    virtual const std::string &get_name() = 0;
    virtual const std::vector<std::byte> &get_image() = 0;
    virtual IMediaEntity::State get_state() const = 0;

    virtual void set_name(const std::string &_name) = 0;
    virtual void set_image(const std::vector<std::byte> &_imageData) = 0;
    virtual void set_state(IMediaEntity::State _state) = 0;

    virtual void ensureLoaded(IMediaService &_service) = 0;
};