#include "Sources/Discogs/DiscogsReleaseSource.h"
#include "Discogs/ReleaseTrack.h"
#include "Interfaces/ITrack.h"

const std::string &DiscogsReleaseSource::get_id() const {
    return release->get_id();
}
const std::string &DiscogsReleaseSource::get_name() {
    return release->get_name();
}
const std::vector<std::byte> &DiscogsReleaseSource::get_image() {
    return release->get_image();
}
std::string DiscogsReleaseSource::get_artist() {
    return release->get_stringArtists();
}
std::string DiscogsReleaseSource::get_copyright() { return {}; }
std::string DiscogsReleaseSource::get_genre() {
    return release->get_stringGenres();
}
std::string DiscogsReleaseSource::get_year() {
    return std::to_string(release->get_year());
}
std::string DiscogsReleaseSource::get_label() {
    return release->get_stringLabels();
}

std::string DiscogsReleaseSource::get_type() const { return "<DiscogsType>"; }
std::vector<std::byte> DiscogsReleaseSource::get_artistCover() {
    return (release->get_artists().empty()
                ? std::vector<std::byte>{}
                : release->get_artists()[0].get_image());
}
std::vector<std::shared_ptr<ITrack>>
DiscogsReleaseSource::get_tracklist() const {
    std::vector<std::shared_ptr<ITrack>> tracklist;
    for (auto &&track : release->get_tracklist()) {
        tracklist.push_back(ITrack::fromDiscogs(
            std::make_shared<Discogs::ReleaseTrack>(track)));
    }
    return tracklist;
}
IMediaEntity::State DiscogsReleaseSource::get_state() const {
    return release->get_state();
}
// bool DiscogsReleaseSource::isMetaDataComplete() const {
//     return release->get_state() == Discogs::Release::State::Full;
// }

void DiscogsReleaseSource::set_name(const std::string &_name) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsReleaseSource::set_image(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsReleaseSource::set_artist(const std::string &_artist) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsReleaseSource::set_copyright(const std::string &_copyright) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsReleaseSource::set_genre(const std::string &_genre) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsReleaseSource::set_year(const std::string &_year) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsReleaseSource::set_label(const std::string &_label) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsReleaseSource::set_state(IMediaEntity::State _state) {
    release->set_state(_state);
}
std::shared_ptr<Discogs::Release> DiscogsReleaseSource::get_release() const {
    return release;
}

void DiscogsReleaseSource::ensureLoaded(IMediaService &_service) {
    if (release->get_state() == IMediaEntity::State::Full)
        return;

    _service.load(*release);
    release->set_state(IMediaEntity::State::Full);
}