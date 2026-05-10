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

    std::string get_id() const override;
    std::string get_title() override;
    std::vector<std::byte> get_cover() override;

    void set_title(const std::string &_title) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;

    static std::shared_ptr<IVideo>
    fromYouTube(std::shared_ptr<YouTube::Video> _video);

  private:
    std::shared_ptr<IVideoSource> source;
};