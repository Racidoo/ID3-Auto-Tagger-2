#include "Discogs/ReleaseTrack.h"
#include "Discogs/Release.h"
#include "Interfaces/IAlbum.h"
#include "Interfaces/IArtist.h"

namespace Discogs {

ReleaseTrack::ReleaseTrack(
    const std::string &_id, const std::string &_name,
    std::weak_ptr<Release> _release,
    const std::vector<std::shared_ptr<IArtist>> &_artists,
    std::size_t _duration, const std::size_t &_discnumber,
    const std::size_t &_tracknumber, IMediaService *_mediaService)
    : ITrack(_id + "_" + std::to_string(_discnumber) + "_" +
                 std::to_string(_tracknumber),
             _name, State::Full, _release.lock()->get_imageUrl(),
             _mediaService),
      release(_release), artists(_artists), duration(_duration),
      discnumber(_discnumber), tracknumber(_tracknumber), verified(false) {}

std::string ReleaseTrack::get_artist() const {
    if (artists.empty()) {
        return release.lock()->get_artist();
    }
    return vecToStr(artists);
}
const std::string &ReleaseTrack::get_albumName() const {
    return release.lock()->get_name();
}
std::string ReleaseTrack::get_albumArtist() const {
    return release.lock()->get_artist();
}
const std::string &ReleaseTrack::get_copyright() const {
    return release.lock()->get_copyright();
}
const std::string &ReleaseTrack::get_genre() const {
    return release.lock()->get_stringGenres();
}
std::size_t ReleaseTrack::get_year() const {
    return release.lock()->get_year();
}
const std::string &ReleaseTrack::get_label() const {
    return release.lock()->get_label();
}
std::size_t ReleaseTrack::get_trackNumber() const { return tracknumber; }
std::size_t ReleaseTrack::get_discNumber() const { return discnumber; }
std::size_t ReleaseTrack::get_length() const { return duration; }

const std::vector<std::byte> &ReleaseTrack::get_image() {
    return release.lock()->get_image();
}

bool ReleaseTrack::is_verified() const { return verified; }
void ReleaseTrack::set_artist(const std::string &_artist) {
    static_assert("Discogs Object is not mutable!");
}
void ReleaseTrack::set_albumName(const std::string &_albumName) {
    static_assert("Discogs Object is not mutable!");
}
void ReleaseTrack::set_albumArtist(const std::string &_albumArtist) {
    static_assert("Discogs Object is not mutable!");
}
void ReleaseTrack::set_copyright(const std::string &_copyright) {
    static_assert("Discogs Object is not mutable!");
}
void ReleaseTrack::set_genre(const std::string &_genre) {
    static_assert("Discogs Object is not mutable!");
}
void ReleaseTrack::set_year(std::size_t _year) {
    static_assert("Discogs Object is not mutable!");
}
void ReleaseTrack::set_label(const std::string &_label) {
    static_assert("Discogs Object is not mutable!");
}
void ReleaseTrack::set_trackNumber(std::size_t _trackNumber) {
    static_assert("Discogs Object is not mutable!");
}
void ReleaseTrack::set_discNumber(std::size_t _discNumber) {
    static_assert("Discogs Object is not mutable!");
}
void ReleaseTrack::set_verified(bool _verified) { verified = _verified; }

std::weak_ptr<IAlbum> ReleaseTrack::get_release() const { return release; }

const std::vector<std::shared_ptr<IArtist>> &ReleaseTrack::get_artists() const {
    return artists;
}

int ReleaseTrack::parseDuration(const std::string &_stringDuration) {
    std::stringstream ss(_stringDuration);

    std::vector<int> parts;
    std::string token;

    while (std::getline(ss, token, ':')) {
        parts.push_back(std::stoi(token));
    }

    if (parts.empty() || parts.size() > 3) {
        return 0;
    }

    int h = 0;
    int m = 0;
    int s = 0;

    if (parts.size() == 3) {
        h = parts[0];
        m = parts[1];
        s = parts[2];
    } else if (parts.size() == 2) {
        m = parts[0];
        s = parts[1];
    } else {
        s = parts[0];
    }

    return s + (m * 60) + (h * 3600);
}
} // namespace Discogs