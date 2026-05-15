#pragma once

#include <memory>

#include "Sources/IVideoSource.hpp"
#include "YouTube/Video.h"

class YouTubeVideoSource : public IVideoSource {
  public:
    explicit YouTubeVideoSource(std::shared_ptr<YouTube::Video> _video)
        : video(std::move(_video)) {}
    ~YouTubeVideoSource() = default;

    const std::string &get_id() const override;
    const std::string &get_name() override;
    const std::vector<std::byte> &get_image() override;
    std::string get_channelTitle() const override;
    std::string get_uploadDate() const override;
    int get_duration() const override;
    bool get_licensed() const override;
    IMediaEntity::State get_state() const;

    void set_name(const std::string &_name) override;
    void set_image(const std::vector<std::byte> &_imageData) override;
    void set_channelTitle(const std::string &_channelTitle) override;
    void set_uploadDate(const std::string &_uploadDate) override;
    void set_duration(int _duration) override;
    void set_licensed(bool _licensed) override;
    void set_state(IMediaEntity::State _state);

    std::shared_ptr<YouTube::Video> get_video() const;

    void ensureLoaded(class IMediaService &_service) override;

  private:
    std::shared_ptr<YouTube::Video> video;
};
