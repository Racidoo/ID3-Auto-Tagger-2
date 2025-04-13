#include "../include/Spotify/User.h"

namespace Spotify {
User::User(const std::string &_id, const std::string &_name,
           const std::string &_imageURL)
    : SpotifyObject(_id, _name, "user", _imageURL) {}

User::~User() {}

} // namespace Spotify
