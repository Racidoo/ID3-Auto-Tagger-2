#include "Spotify/User.h"

namespace Spotify {
User::User(const std::string &_id, const std::string &_name, State _state,
           const std::string &_imageURL, IMediaService *_mediaService)
    : IArtist(_id, _state, _mediaService), name(_name),
      imageProvider(_imageURL) {}

const std::string &User::get_name() const { return name; }
std::vector<std::byte> User::get_image() { return imageProvider.get_image(); }
} // namespace Spotify
