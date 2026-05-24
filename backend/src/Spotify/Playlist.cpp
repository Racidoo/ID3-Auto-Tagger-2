#include "Spotify/Playlist.h"
#include "Spotify/User.h"

namespace Spotify {

Playlist::Playlist(const std::string &_id, const std::string &_name,
                   State _state, const std::string &_imageURL,
                   std::shared_ptr<User> _creator, IMediaService *_mediaService)
    : IPlaylist(_id, _state, _mediaService), name(_name),
      imageProvider(_imageURL), creator(_creator) {}

std::optional<std::string> Playlist::get_name() const { return name; }
std::optional<std::vector<std::byte>> Playlist::get_image() {
    return imageProvider.get_image();
}
} // namespace Spotify
