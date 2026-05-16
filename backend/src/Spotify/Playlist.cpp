#include "Spotify/Playlist.h"

namespace Spotify {

Playlist::Playlist(const std::string &_id, const std::string &_name,
                   State _state, const std::string &_imageURL,
                   const User &_creator)
    : IPlaylist(_id, _name, _state, _imageURL), creator(_creator) {}

} // namespace Spotify
