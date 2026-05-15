#include "Sources/YouTube/YouTubeVideoSource.h"

const std::string &YouTubeVideoSource::get_id() const {
    return video->get_id();
}
const std::string &YouTubeVideoSource::get_name() { return video->get_name(); }
const std::vector<std::byte> &YouTubeVideoSource::get_image() {
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
IMediaEntity::State YouTubeVideoSource::get_state() const {
    return video->get_state();
}

void YouTubeVideoSource::set_name(const std::string &_name) {
    throw std::logic_error("YouTub::Video Object is not mutable!");
}
void YouTubeVideoSource::set_image(const std::vector<std::byte> &_imageData) {
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
void YouTubeVideoSource::set_state(IMediaEntity::State _state) {
    throw std::logic_error("YouTub::Video Object is not mutable!");
}

void YouTubeVideoSource::ensureLoaded(IMediaService &_service) {
    if (video->get_state() == IMediaEntity::State::Full)
        return;

    _service.load(*video);
    video->set_state(IMediaEntity::State::Full);
}
