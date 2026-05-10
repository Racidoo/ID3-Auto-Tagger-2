#include "../include/Spotify/Album.h"

namespace Spotify {

Album::Album(const std::string &_id, const std::string &_name,
             MetadataState _state, const std::string &_albumType,
             const unsigned int _totalTracks, const std::string &_releaseDate,
             const std::string &_imageURL, const std::vector<Artist> &_artists)
    : QueryObject(_id, _name, _state, _imageURL), albumType(_albumType),
      totalTracks(_totalTracks), releaseDate(_releaseDate), label(""),
      copyright(""), artists(_artists) {}

const std::string &Album::get_albumType() const { return albumType; }
unsigned int Album::get_totalTracks() const { return totalTracks; }
const std::string Album::get_releaseDate() const { return releaseDate; }
std::string Album::get_releaseYear() const {
    return get_releaseDate().substr(0, 4);
}
const std::string &Album::get_label() const { return label; }
const std::string &Album::get_copyright() const { return copyright; }
std::string Album::get_artists() const { return vecToStr(artists); }

void Album::set_label(const std::string &_label) { label = _label; }
void Album::set_copyright(const std::string &_copyright) {
    copyright = _copyright;
}

} // namespace Spotify