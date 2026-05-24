#include "Spotify/Artist.h"

namespace Spotify {

Artist::Artist(const std::string &_id, const std::string &_name, State _state,
               const std::string &_imageURL, IMediaService *_mediaService)
    : IArtist(_id, _state, _mediaService), name(_name),
      imageProvider(_imageURL) {}

std::optional<std::string> Artist::get_name() const { return name; }
std::optional<std::vector<std::byte>> Artist::get_image() {
    return imageProvider.get_image();
}
} // namespace Spotify