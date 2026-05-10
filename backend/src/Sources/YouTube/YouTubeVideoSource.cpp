#include "Sources/YouTube/YouTubeVideoSource.h"

std::string YouTubeVideoSource::get_id() const { return video->get_id(); }
std::string YouTubeVideoSource::get_title() { return video->get_name(); }
std::vector<std::byte> YouTubeVideoSource::get_cover() {
    return video->get_image();
}
std::string YouTubeVideoSource::get_channelTitle() const {
    return video->get_channelTitle();
}
std::string YouTubeVideoSource::get_uploadDate() const {
    return video->get_uploadDate();
}
int YouTubeVideoSource::get_duration() const { return video->get_duration(); }
bool YouTubeVideoSource::get_licensed() const { return video->get_licensed(); }
std::shared_ptr<YouTube::Video> YouTubeVideoSource::get_video() const {
    return video;
}

void YouTubeVideoSource::set_title(const std::string &_title) {
    throw std::logic_error("YouTub::Video Object is not mutable!");
}
void YouTubeVideoSource::set_cover(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("YouTub::Video Object is not mutable!");
}
void YouTubeVideoSource::set_channelTitle(const std::string &_channelTitle) {
    throw std::logic_error("YouTub::Video Object is not mutable!");
}
void YouTubeVideoSource::set_uploadDate(const std::string &_uploadDate) {
    throw std::logic_error("YouTub::Video Object is not mutable!");
}
void YouTubeVideoSource::set_duration(int _duration) {
    throw std::logic_error("YouTub::Video Object is not mutable!");
}
void YouTubeVideoSource::set_licensed(bool _licensed) {
    throw std::logic_error("YouTub::Video Object is not mutable!");
}
