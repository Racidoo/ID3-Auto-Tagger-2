#include "../include/Spotify/Artist.h"

namespace Spotify {

Artist::Artist(const std::string &_id, const std::string &_name)
    : SpotifyObject(_id, _name, "artist") {}

Artist::~Artist() {}

std::string Artist::vecToStr(const std::vector<Artist> &_artists,
                             const std::string &_sep) {
    std::stringstream stringArtists;
    for (size_t i = 0; i < _artists.size(); i++) {
        stringArtists << _artists[i].get_name()
                      << (i < _artists.size() - 1 ? _sep : "");
    }
    return stringArtists.str();
}

} // namespace Spotify