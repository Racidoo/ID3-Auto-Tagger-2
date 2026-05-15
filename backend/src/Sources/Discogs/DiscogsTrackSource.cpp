#include "Sources/Discogs/DiscogsTrackSource.h"

const std::string &DiscogsTrackSource::get_id() const {
    return track->get_id();
}
const std::string &DiscogsTrackSource::get_name() { return track->get_name(); }
std::string DiscogsTrackSource::get_artist() {
    return track->get_stringArtists();
}
std::string DiscogsTrackSource::get_album() {
    return track->get_release().get_name();
}
std::string DiscogsTrackSource::get_albumArtist() {
    return track->get_release().get_stringArtists();
}
std::string DiscogsTrackSource::get_copyright() { return {}; }
std::string DiscogsTrackSource::get_genre() {
    return track->get_stringGenres();
}
std::string DiscogsTrackSource::get_year() {
    return std::to_string(track->get_year());
}
std::string DiscogsTrackSource::get_label() {
    return track->get_stringLabels();
}
std::string DiscogsTrackSource::get_trackNumber() { return {}; }
std::string DiscogsTrackSource::get_discNumber() { return {}; }
std::size_t DiscogsTrackSource::get_length() { return track->get_duration(); }
const std::vector<std::byte> &DiscogsTrackSource::get_image() {
    return track->get_image();
}
IMediaEntity::State DiscogsTrackSource::get_state() const {
    return IMediaEntity::State::Full;
}
std::shared_ptr<Discogs::ReleaseTrack> DiscogsTrackSource::get_track() const {
    return track;
}

void DiscogsTrackSource::set_name(const std::string &_name) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_artist(const std::string &_artist) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_album(const std::string &_album) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_albumArtist(const std::string &_albumArtist) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_copyright(const std::string &_copyright) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_genre(const std::string &_genre) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_year(const std::string &_year) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_label(const std::string &_label) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_trackNumber(const std::string &_track) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_discNumber(const std::string &_disc) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_image(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsTrackSource::set_state(IMediaEntity::State _state) {
    throw std::logic_error("Discogs Object is not mutable!");
}

void DiscogsTrackSource::ensureLoaded(IMediaService &_service) {
    // if (track->get_state() == IMediaEntity::State::Full)
        return;

    // _service.load(*track);
    // track->set_state(IMediaEntity::State::Full);
}