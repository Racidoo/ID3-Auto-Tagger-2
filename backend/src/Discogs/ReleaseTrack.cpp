#include "Discogs/ReleaseTrack.h"
#include "Discogs/Release.h"
#include "Interfaces/IAlbum.h"
#include "Interfaces/IArtist.h"

namespace Discogs {

ReleaseTrack::ReleaseTrack(
    const std::string &_id, const std::optional<std::string> &_name,
    std::weak_ptr<Release> _release,
    const std::vector<std::shared_ptr<IArtist>> &_artists,
    std::size_t _duration, const std::optional<std::size_t> &_discnumber,
    const std::optional<std::size_t> &_tracknumber,
    IMediaService *_mediaService)
    : ITrack(_id + "_" +
                 (_discnumber.has_value() ? std::to_string(_discnumber.value())
                                          : "") +
                 "_" +
                 (_tracknumber.has_value()
                      ? std::to_string(_tracknumber.value())
                      : ""),
             State::Full, _mediaService),
      name(_name), release(_release), artists(_artists), duration(_duration),
      discnumber(_discnumber), tracknumber(_tracknumber), verified(false) {}
std::optional<std::string> ReleaseTrack::get_name() const { return name; }
std::optional<std::string> ReleaseTrack::get_artist() const {
    if (artists.empty()) {
        auto _release = release.lock();
        if (!_release)
            return std::nullopt;
        return _release->get_artist();
    }
    return vecToStr(artists);
}
std::optional<std::string> ReleaseTrack::get_albumName() const {
    auto _release = release.lock();
    if (!_release)
        return std::nullopt;
    return _release->get_name();
}
std::optional<std::string> ReleaseTrack::get_albumArtist() const {
    auto _release = release.lock();
    if (!_release)
        return std::nullopt;
    return _release->get_artist();
}
std::optional<std::string> ReleaseTrack::get_copyright() const {
    auto _release = release.lock();
    if (!_release)
        return std::nullopt;
    return _release->get_copyright();
}
std::optional<std::string> ReleaseTrack::get_genre() const {
    auto _release = release.lock();
    if (!_release)
        return std::nullopt;
    return _release->get_stringGenres();
}
std::optional<size_t> ReleaseTrack::get_year() const {
    auto _release = release.lock();
    if (!_release)
        return std::nullopt;
    return _release->get_year();
}
std::optional<std::string> ReleaseTrack::get_label() const {
    auto _release = release.lock();
    if (!_release)
        return std::nullopt;
    return _release->get_label();
}
std::optional<size_t> ReleaseTrack::get_trackNumber() const {
    return tracknumber;
}
std::optional<size_t> ReleaseTrack::get_discNumber() const {
    return discnumber;
}
std::size_t ReleaseTrack::get_length() const { return duration; }

std::optional<std::vector<std::byte>> ReleaseTrack::get_image() {
    auto _release = release.lock();
    if (!_release)
        return std::nullopt;
    return _release->get_image();
}

std::weak_ptr<IAlbum> ReleaseTrack::get_release() const { return release; }
const std::vector<std::shared_ptr<IArtist>> &ReleaseTrack::get_artists() const {
    return artists;
}

bool ReleaseTrack::is_verified() const { return verified; }
void ReleaseTrack::set_verified(bool _verified) { verified = _verified; }

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