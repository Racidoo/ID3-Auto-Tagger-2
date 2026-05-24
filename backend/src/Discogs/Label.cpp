#include "Discogs/Label.h"

namespace Discogs {

Label::Label(int _id, const std::optional<std::string> &_name, State _state,
             const std::string &_imageUrl, IMediaService *_mediaService)
    : MediaEntityBase(std::to_string(_id), _state, _mediaService), name(_name),
      imageProvider(_imageUrl) {}

std::optional<std::string> Label::get_name() const { return name; }
std::optional<std::vector<std::byte>> Label::get_image() {
    return imageProvider.get_image();
}
} // namespace Discogs
