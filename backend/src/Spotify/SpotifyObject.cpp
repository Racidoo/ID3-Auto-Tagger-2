#include "../include/Spotify/SpotifyObject.h"

namespace Spotify {

SpotifyObject::SpotifyObject(const std::string &_id, const std::string &_name,
                             const std::string &_type,
                             const std::string &_imageURL)
    : id(_id), name(_name), type(_type), imageURL(_imageURL) {}

SpotifyObject::~SpotifyObject() {}

} // namespace Spotify