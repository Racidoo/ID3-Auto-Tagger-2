#include "Spotify/Album.h"
#include "Spotify/Artist.h"
#include "Spotify/SpotifyAPI.h"
#include "Spotify/Track.h"

namespace Spotify {

Album::Album(const std::string &_id, const std::string &_name, State _state,
             IAlbum::album_type_t _albumType, std::size_t _totalTracks,
             const std::string &_releaseDate, std::size_t _year,
             const std::string &_imageURL,
             const std::vector<std::shared_ptr<IArtist>> &_artists,
             IMediaService *_mediaService)
    : IAlbum(_id, _name, _state, _imageURL, _mediaService), type(_albumType),
      year(_year), totalTracks(_totalTracks), releaseDate(_releaseDate),
      label(""), copyright(""), artists(_artists), tracklist{} {}

IAlbum::album_type_t Album::get_type() const { return type; }
std::string Album::get_artist() const { return vecToStr(artists); }
std::vector<std::byte> Album::get_artistImage() {
    if (artists.empty()) {
        return std::vector<std::byte>{};
    }
    return artists[0]->get_image();
}
std::size_t Album::get_year() const { return year; }
std::size_t Album::get_totalTracks() const { return totalTracks; }
const std::string Album::get_releaseDate() const { return releaseDate; }
const std::string &Album::get_label() const { return label; }
const std::string &Album::get_copyright() const { return copyright; }
const std::vector<std::shared_ptr<IArtist>> &Album::get_artists() const {
    return artists;
}
std::vector<std::shared_ptr<IArtist>> &Album::get_artists() { return artists; }
const std::vector<std::shared_ptr<ITrack>> &Album::get_tracklist() const {
    return tracklist;
}

void Album::set_label(const std::string &_label) { label = _label; }
void Album::set_copyright(const std::string &_copyright) {
    copyright = _copyright;
}
void Album::set_tracklist(
    const std::vector<std::shared_ptr<ITrack>> &_tracklist) {
    tracklist = _tracklist;
}
void Album::set_year(std::size_t _year) { year = _year; }

} // namespace Spotify