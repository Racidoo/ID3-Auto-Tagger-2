#include "Interfaces/IVideo.h"
#include "Sources/YouTube/YouTubeVideoSource.h"

std::string IVideo::get_id() const { return source ? source->get_id() : ""; }
std::string IVideo::get_title() { return source ? source->get_title() : ""; }
std::vector<std::byte> IVideo::get_cover() {
    return source ? source->get_cover() : std::vector<std::byte>{};
}

void IVideo::set_title(const std::string &_title) {
    if (source)
        source->set_title(_title);
}
void IVideo::set_cover(const std::vector<std::byte> &_imageData) {
    if (source)
        source->set_cover(_imageData);
}

std::shared_ptr<IVideo>
IVideo::fromYouTube(std::shared_ptr<YouTube::Video> _release) {
    return std::make_shared<IVideo>(
        std::make_shared<YouTubeVideoSource>(_release));
}