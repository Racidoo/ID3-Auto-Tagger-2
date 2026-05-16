#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "Interfaces/IVideo.h"

namespace YouTube {
class Video : public IVideo {
  public:
    Video(const std::string &_id, const std::string &_name, State _state,
          const std::string &_channelTitle, const std::string &_uploadDate,
          const std::string &_imageUrl, unsigned int _duration, bool _licensed);
    ~Video() = default;

    const std::string &get_channelTitle() const;
    const std::string &get_uploadDate() const;
    unsigned int get_duration() const;
    bool get_licensed() const;

  private:
    std::string channelTitle;
    std::string uploadDate;
    unsigned int duration;
    bool licensed;
    unsigned int views;
};

} // namespace YouTube
