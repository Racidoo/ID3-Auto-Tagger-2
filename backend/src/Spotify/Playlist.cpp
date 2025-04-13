#include "../include/Spotify/Playlist.h"

namespace Spotify {

Playlist::Playlist(const std::string &_id, const std::string &_name,
                   const std::string &_imageURL, const User &_creator)
    : SpotifyObject(_id, _name, "playlist", _imageURL), creator(_creator) {}

Playlist::~Playlist() {}
} // namespace Spotify
