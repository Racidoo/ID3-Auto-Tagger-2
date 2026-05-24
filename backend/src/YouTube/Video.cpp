#include "YouTube/Video.h"

namespace YouTube {

Video::Video(const std::string &_id, const std::string &_name, State _state,
             const std::string &_channelTitle, const std::string &_uploadDate,
             const std::string &_imageUrl, std::size_t _duration,
             bool _licensed, IMediaService *_mediaService)
    : IVideo(_id, _state, _mediaService), name(_name), imageProvider(_imageUrl),
      channelTitle(_channelTitle), uploadDate(_uploadDate), duration(_duration),
      licensed(_licensed) {}

std::optional<std::string> Video::get_name() const { return name; }
std::optional<std::vector<std::byte>> Video::get_image() {
    return imageProvider.get_image();
}
const std::string &Video::get_channelTitle() const { return channelTitle; }
const std::string &Video::get_uploadDate() const { return uploadDate; }
std::size_t Video::get_length() const { return duration; }
bool Video::get_licensed() const { return licensed; }

} // namespace YouTube
