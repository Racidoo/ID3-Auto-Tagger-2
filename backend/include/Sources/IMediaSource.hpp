#pragma once

#include <string>
#include <vector>

class IMediaSource {
  public:
    ~IMediaSource() = default;

    virtual std::string get_id() const = 0;
    virtual std::string get_title() = 0;
    virtual std::vector<std::byte> get_cover() = 0;

    virtual void set_title(const std::string &_title) = 0;
    virtual void set_cover(const std::vector<std::byte> &_imageData) = 0;
};