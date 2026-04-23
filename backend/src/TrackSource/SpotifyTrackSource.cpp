#include "TrackSource/SpotifyTrackSource.h"

std::string SpotifyTrackSource::get_id() const { return track->get_id(); }
std::string SpotifyTrackSource::get_title() { return track->get_name(); }
std::string SpotifyTrackSource::get_artist() {
    return track->get_stringArtists();
}
std::string SpotifyTrackSource::get_album() {
    return track->get_album().get_name();
}
std::string SpotifyTrackSource::get_albumArtist() {
    return track->get_album().get_artists();
}
std::string SpotifyTrackSource::get_copyright() {
    return track->get_album().get_copyright();
}
std::string SpotifyTrackSource::get_genre() { return "Unknown"; }
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
std::vector<std::byte> SpotifyTrackSource::get_cover() {
    return track->get_album().get_image();
}
std::shared_ptr<Spotify::Track> SpotifyTrackSource::get_track() const {
    return track;
}

void SpotifyTrackSource::set_title(const std::string &_title) {
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
    throw std::logic_error("Spotify Object is not mutable!");
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
void SpotifyTrackSource::set_cover(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Spotify Object is not mutable!");
}
