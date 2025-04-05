#include "../include/Spotify/SpotifyObject.h"

namespace Spotify {

SpotifyObject::SpotifyObject(const std::string &_id, const std::string &_name,
                             const std::string &_type)
    : id(_id), name(_name), type(_type) {}

SpotifyObject::~SpotifyObject() {}

} // namespace Spotify