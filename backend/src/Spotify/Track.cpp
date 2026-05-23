#include "Spotify/Track.h"
#include "Interfaces/IArtist.h"

namespace Spotify {

Track::Track(const std::string &_id, const std::string &_name, State _state,
             std::size_t _discNumber, std::size_t _length,
             bool _explicitContent, std::size_t _trackNumber,
             std::shared_ptr<Album> _album,
             const std::vector<std::shared_ptr<IArtist>> &_artists,
             IMediaService *_mediaService)
    : ITrack(_id, _state, _mediaService), name(_name),
      imageProvider(_album->get_imageProvider().get_imageUrl(),
                    _album->get_image()),
      discNumber(_discNumber), length(_length),
      explicitContent(_explicitContent), trackNumber(_trackNumber),
      album(_album), artists(_artists), genre(""), downloaded(false) {}

const std::string &Track::get_name() const { return name; }
const std::vector<std::shared_ptr<IArtist>> &Track::get_artists() {
    return artists;
}
std::shared_ptr<IAlbum> Track::get_album() { return album; }
std::string Track::get_artist() const { return vecToStr(artists); }
std::size_t Track::get_trackNumber() const { return trackNumber; }
std::size_t Track::get_discNumber() const { return discNumber; }
std::size_t Track::get_length() const { return length; }
const std::string &Track::get_albumName() const { return album->get_name(); }
std::string Track::get_albumArtist() const { return album->get_artist(); }
const std::string &Track::get_copyright() const {
    return album->get_copyright();
}
const std::string &Track::get_genre() const { return genre; }
std::size_t Track::get_year() const { return album->get_year(); }
const std::string &Track::get_label() const { return album->get_label(); }
bool Track::is_verified() const { return downloaded; }
std::vector<std::byte> Track::get_image() { return imageProvider.get_image(); }

void Track::set_name(const std::string &_name) {
    static_assert("Spotify Object is not mutable!");
}
void Track::set_artist(const std::string &_artist) {
    static_assert("Spotify Object is not mutable!");
}
void Track::set_albumName(const std::string &_albumName) {
    static_assert("Spotify Object is not mutable!");
}
void Track::set_albumArtist(const std::string &_albumArtist) {
    static_assert("Spotify Object is not mutable!");
}
void Track::set_copyright(const std::string &_copyright) {
    album->set_copyright(_copyright);
}
void Track::set_genre(const std::string &_genre) { genre = _genre; }
void Track::set_year(std::size_t _year) { album->set_year(_year); }
void Track::set_label(const std::string &_label) { album->set_label(_label); }
void Track::set_trackNumber(std::size_t _trackNumber) {
    static_assert("Spotify Object is not mutable!");
}
void Track::set_discNumber(std::size_t _discNumber) {
    static_assert("Spotify Object is not mutable!");
}
void Track::set_verified(bool _verified) { downloaded = _verified; }
void Track::set_image(const std::vector<std::byte> &_imageData) {
    imageProvider.set_image(_imageData);
}
// const std::string &Track::get_genre() const { return genre; }

// bool Track::isDownloaded() const { return downloaded; }

// void Track::set_genre(const std::string &_genre) { genre = _genre; }
// void Track::set_downloaded(bool _downloaded) { downloaded = _downloaded; }

} // namespace Spotify