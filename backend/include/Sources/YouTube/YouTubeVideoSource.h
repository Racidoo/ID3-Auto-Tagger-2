#pragma once

#include <memory>

#include "Sources/IVideoSource.hpp"
#include "YouTube/Video.h"

class YouTubeVideoSource : public IVideoSource {
  public:
    explicit YouTubeVideoSource(std::shared_ptr<YouTube::Video> _video)
        : video(std::move(_video)) {}
    ~YouTubeVideoSource() = default;

    std::string get_id() const override;
    std::string get_title() override;
    std::vector<std::byte> get_cover() override;
    std::string get_channelTitle() const override;
    std::string get_uploadDate() const override;
    int get_duration() const override;
    bool get_licensed() const override;

    void set_title(const std::string &_title) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;
    void set_channelTitle(const std::string &_channelTitle) override;
    void set_uploadDate(const std::string &_uploadDate) override;
    void set_duration(int _duration) override;
    void set_licensed(bool _licensed) override;

    std::shared_ptr<YouTube::Video> get_video() const;

  private:
    std::shared_ptr<YouTube::Video> video;
};
