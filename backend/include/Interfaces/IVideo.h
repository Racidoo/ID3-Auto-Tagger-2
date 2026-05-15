#pragma once

#include <memory>

#include "IMediaEntity.hpp"

class IVideoSource;

namespace YouTube {
class Video;
} // namespace YouTube

class IVideo : public IMediaEntity {
  public:
    explicit IVideo(std::shared_ptr<IVideoSource> _source)
        : source(std::move(_source)) {}
    ~IVideo() = default;

    const std::string &get_id() const override;
    const std::string &get_name() const override;
    const std::vector<std::byte> &get_image() override;

    void ensureLoaded(class IMediaService &_service) override;

    static std::shared_ptr<IVideo>
    fromYouTube(std::shared_ptr<YouTube::Video> _video);

  private:
    std::shared_ptr<IVideoSource> source;
};