#include "Interfaces/IVideo.h"
#include "IMediaService.hpp"
#include "Sources/YouTube/YouTubeVideoSource.h"

const std::string &IVideo::get_id() const { return source->get_id(); }
const std::string &IVideo::get_name() const { return source->get_name(); }
const std::vector<std::byte> &IVideo::get_image() {
    return source->get_image();
}

void IVideo::ensureLoaded(class IMediaService &_service) {
    source->ensureLoaded(_service);
}
std::shared_ptr<IVideo>
IVideo::fromYouTube(std::shared_ptr<YouTube::Video> _release) {
    return std::make_shared<IVideo>(
        std::make_shared<YouTubeVideoSource>(_release));
}