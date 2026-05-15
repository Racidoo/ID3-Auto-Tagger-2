#include "Spotify/Album.h"
#include "Spotify/SpotifyAPI.h"
#include "Spotify/Track.h"

namespace Spotify {

Album::Album(const std::string &_id, const std::string &_name,
            State _state, const std::string &_albumType,
             const unsigned int _totalTracks, const std::string &_releaseDate,
             const std::string &_imageURL, const std::vector<Artist> &_artists)
    : MediaEntityBase(_id, _name, _state, _imageURL), albumType(_albumType),
      totalTracks(_totalTracks), releaseDate(_releaseDate), label(""),
      copyright(""), artists(_artists), tracklist{} {}

const std::string &Album::get_albumType() const { return albumType; }
unsigned int Album::get_totalTracks() const { return totalTracks; }
const std::string Album::get_releaseDate() const { return releaseDate; }
std::string Album::get_releaseYear() const {
    return get_releaseDate().substr(0, 4);
}
const std::string &Album::get_label() const { return label; }
const std::string &Album::get_copyright() const { return copyright; }
const std::vector<Artist> &Album::get_artists() const { return artists; }
std::vector<Artist> &Album::get_artists() { return artists; }
std::string Album::get_stringArtists() const { return vecToStr(artists); }
const std::vector<Track> &Album::get_tracklist() const { return tracklist; }

void Album::set_label(const std::string &_label) { label = _label; }
void Album::set_copyright(const std::string &_copyright) {
    copyright = _copyright;
}
void Album::set_tracklist(const std::vector<Track> &_tracklist) {
    tracklist = _tracklist;
}

// void Album::loadAdditionalData(std::weak_ptr<IMediaService> _service) {
//     if (state == MetadataState::Full)
//         return;

//     _service.lock()->load(*this);
//     state = MetadataState::Full;
// }

} // namespace Spotify