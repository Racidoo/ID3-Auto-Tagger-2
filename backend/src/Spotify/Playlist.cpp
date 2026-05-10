#include "../include/Spotify/Playlist.h"

namespace Spotify {

Playlist::Playlist(const std::string &_id, const std::string &_name,
                   MetadataState _state, const std::string &_imageURL,
                   const User &_creator)
    : QueryObject(_id, _name, _state, _imageURL), creator(_creator) {}

Playlist::~Playlist() {}
} // namespace Spotify
