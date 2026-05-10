#pragma once

#include <string>
#include <vector>

#include "IMediaSource.hpp"

class IVideoSource : public IMediaSource {
  public:
    virtual ~IVideoSource() = default;

    virtual std::string get_channelTitle() const = 0;
    virtual std::string get_uploadDate() const = 0;
    virtual int get_duration() const = 0;
    virtual bool get_licensed() const = 0;

    virtual void set_channelTitle(const std::string &_channelTitle) = 0;
    virtual void set_uploadDate(const std::string &_uploadDate) = 0;
    virtual void set_duration(int _duration) = 0;
    virtual void set_licensed(bool _licensed) = 0;
};
