#include "../include/Spotify/User.h"

namespace Spotify {
User::User(const std::string &_id, const std::string &_name,
           MetadataState _state, const std::string &_imageURL)
    : QueryObject(_id, _name, _state, _imageURL) {}

User::~User() {}

} // namespace Spotify
