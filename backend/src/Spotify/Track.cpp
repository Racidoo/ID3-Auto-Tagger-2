#include "Spotify/Track.h"
#include "Interfaces/IArtist.h"

namespace Spotify {
Track::Track(const std::string &_id, const std::optional<std::string> &_name,
             State _state, std::optional<std::size_t> _discNumber,
             std::size_t _length, bool _explicitContent,
             std::optional<std::size_t> _trackNumber,
             std::weak_ptr<Album> _album,
             const std::vector<std::shared_ptr<IArtist>> &_artists,
             IMediaService *_mediaService)
    : ITrack(_id, _state, _mediaService), name(_name), discNumber(_discNumber),
      length(_length), explicitContent(_explicitContent),
      trackNumber(_trackNumber), album(_album), artists(_artists),
      downloaded(false) {}

std::optional<std::string> Track::get_name() const { return name; }
std::vector<std::shared_ptr<IArtist>> Track::get_artists() const {
    return artists;
}
std::weak_ptr<IAlbum> Track::get_album() { return album; }
std::optional<std::string> Track::get_artist() const {
    return vecToStr(artists);
}
std::optional<size_t> Track::get_trackNumber() const { return trackNumber; }
std::optional<size_t> Track::get_discNumber() const { return discNumber; }
std::size_t Track::get_length() const { return length; }
std::optional<std::string> Track::get_albumName() const {
    auto _album = album.lock();
    if (!_album)
        return std::nullopt;
    return _album->get_name();
}
std::optional<std::string> Track::get_albumArtist() const {
    auto _album = album.lock();
    if (!_album)
        return std::nullopt;
    return _album->get_artist();
}
std::optional<std::string> Track::get_copyright() const {
    auto _album = album.lock();
    if (!_album)
        return std::nullopt;
    return _album->get_copyright();
}
std::optional<std::string> Track::get_genre() const { return genre; }
std::optional<size_t> Track::get_year() const {
    auto _album = album.lock();
    if (!_album)
        return std::nullopt;
    return _album->get_year();
}
std::optional<std::string> Track::get_label() const {
    auto _album = album.lock();
    if (!_album)
        return std::nullopt;
    return _album->get_label();
}
std::optional<std::vector<std::byte>> Track::get_image() {
    auto _album = album.lock();
    if (!_album)
        return std::nullopt;
    return _album->get_image();
}

bool Track::is_verified() const { return downloaded; }
void Track::set_verified(bool _verified) { downloaded = _verified; }

} // namespace Spotify