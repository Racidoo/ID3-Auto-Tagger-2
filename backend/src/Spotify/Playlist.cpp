#include "Spotify/Playlist.h"
#include "Spotify/User.h"

namespace Spotify {

Playlist::Playlist(const std::string &_id, const std::string &_name,
                   State _state, const std::string &_imageURL,
                   std::shared_ptr<User> _creator, IMediaService *_mediaService)
    : IPlaylist(_id, _name, _state, _imageURL, _mediaService),
      creator(_creator) {}

} // namespace Spotify
