#include "../include/Spotify/Album.h"

namespace Spotify {

Album::Album(const std::string &_id, const std::string &_name,
             const std::string &_albumType, const unsigned int _totalTracks,
             const std::string &_releaseDate, const std::string &_imageURL,
             const std::vector<Artist> &_artists)
    : SpotifyObject(_id, _name, "album"), albumType(_albumType),
      totalTracks(_totalTracks), releaseDate(_releaseDate), imageURL(_imageURL),
      label(""), copyright(""), artists(_artists) {}

Album::~Album() {}

} // namespace Spotify