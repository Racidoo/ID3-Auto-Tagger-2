#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "Interfaces/IRemoteImageProvider.h"
#include "Interfaces/IVideo.h"

namespace YouTube {
class Video : public IVideo {
  public:
    Video(const std::string &_id, const std::string &_name, State _state,
          const std::string &_channelTitle, const std::string &_uploadDate,
          const std::string &_imageUrl, std::size_t _duration, bool _licensed,
          IMediaService *_mediaService);
    ~Video() = default;

    std::optional<std::string> get_name() const override;
    std::optional<std::vector<std::byte>> get_image() override;
    const std::string &get_channelTitle() const;
    const std::string &get_uploadDate() const;
    std::size_t get_length() const override;
    bool get_licensed() const;

  private:
    std::optional<std::string> name;
    IRemoteImageProvider imageProvider;
    std::string channelTitle;
    std::string uploadDate;
    std::size_t duration;
    bool licensed;
    std::size_t views;
};

} // namespace YouTube
