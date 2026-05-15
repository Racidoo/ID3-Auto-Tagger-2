#include "Sources/Spotify/SpotifyTrackSource.h"

const std::string &SpotifyTrackSource::get_id() const {
    return track->get_id();
}
const std::string &SpotifyTrackSource::get_name() { return track->get_name(); }
std::string SpotifyTrackSource::get_artist() {
    return track->get_stringArtists();
}
std::string SpotifyTrackSource::get_album() {
    return track->get_album().get_name();
}
std::string SpotifyTrackSource::get_albumArtist() {
    return track->get_album().get_stringArtists();
}
std::string SpotifyTrackSource::get_copyright() {
    return track->get_album().get_copyright();
}
std::string SpotifyTrackSource::get_genre() { return track->get_genre(); }
std::string SpotifyTrackSource::get_year() {
    return track->get_album().get_releaseYear();
}
std::string SpotifyTrackSource::get_label() {
    return track->get_album().get_label();
}
std::string SpotifyTrackSource::get_trackNumber() {
    return std::to_string(track->get_trackNumber());
}
std::string SpotifyTrackSource::get_discNumber() {
    return std::to_string(track->get_discNumber());
}
std::size_t SpotifyTrackSource::get_length() {
    return track->get_durationMs() / 1000;
}
const std::vector<std::byte> &SpotifyTrackSource::get_image() {
    return track->get_album().get_image();
}
IMediaEntity::State SpotifyTrackSource::get_state() const {
    return track->get_state();
}
std::shared_ptr<Spotify::Track> SpotifyTrackSource::get_track() const {
    return track;
}

void SpotifyTrackSource::set_name(const std::string &_name) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyTrackSource::set_artist(const std::string &_artist) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyTrackSource::set_album(const std::string &_album) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyTrackSource::set_albumArtist(const std::string &_albumArtist) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyTrackSource::set_copyright(const std::string &_copyright) {
    track->get_album().set_copyright(_copyright);
}
void SpotifyTrackSource::set_genre(const std::string &_genre) {
    track->set_genre(_genre);
}
void SpotifyTrackSource::set_year(const std::string &_year) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyTrackSource::set_label(const std::string &_label) {
    track->get_album().set_label(_label);
}
void SpotifyTrackSource::set_trackNumber(const std::string &_track) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyTrackSource::set_discNumber(const std::string &_disc) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyTrackSource::set_image(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyTrackSource::set_state(IMediaEntity::State _state) {
    track->set_state(_state);
}

void SpotifyTrackSource::ensureLoaded(IMediaService &_service) {
    if (track->get_state() == IMediaEntity::State::Full)
        return;

    _service.load(*track);
    track->set_state(IMediaEntity::State::Full);
}