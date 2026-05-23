#pragma once

#include <vector>

class IImageProvider {
  public:
    virtual ~IImageProvider() = default;

    virtual std::vector<std::byte> get_image() = 0;
    virtual void set_image(const std::vector<std::byte> &_imageData) = 0;
};