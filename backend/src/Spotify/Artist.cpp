#include "../include/Spotify/Artist.h"

namespace Spotify {

Artist::Artist(const std::string &_id, const std::string &_name,const std::string &_imageURL)
    : SpotifyObject(_id, _name, "artist", _imageURL) {}

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