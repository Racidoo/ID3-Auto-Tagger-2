#include "../include/Spotify/SpotifyObject.h"
#include "../../include/Query.h"

namespace Spotify {

SpotifyObject::SpotifyObject(const std::string &_id, const std::string &_name,
                             const std::string &_type,
                             const std::string &_imageURL)
    : id(_id), name(_name), type(_type), imageURL(_imageURL) {}

SpotifyObject::~SpotifyObject() {}

std::vector<std::byte> SpotifyObject::get_image() const {
    return Query::downloadImage(imageURL);
}

bool SpotifyObject::isValidIdFormat(const std::string &_id) {
    static const std::regex pattern("^[A-Za-z0-9]{22}$");
    return std::regex_match(_id, pattern);
}

} // namespace Spotify