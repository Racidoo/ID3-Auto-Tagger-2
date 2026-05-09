#include "TrackSource/DiscogsTrackSource.h"

std::string DiscogsTrackSource::get_id() const { return track->get_id(); }
std::string DiscogsTrackSource::get_title() { return track->get_name(); }
std::string DiscogsTrackSource::get_artist() { return {}; }
std::string DiscogsTrackSource::get_album() { return {}; }
std::string DiscogsTrackSource::get_albumArtist() { return {}; }
std::string DiscogsTrackSource::get_copyright() { return {}; }
std::string DiscogsTrackSource::get_genre() { return track->genres; }
std::string DiscogsTrackSource::get_year() { return track->year; }
std::string DiscogsTrackSource::get_label() { return {}; }
std::string DiscogsTrackSource::get_trackNumber() { return {}; }
std::string DiscogsTrackSource::get_discNumber() { return {}; }
std::size_t DiscogsTrackSource::get_length() { return {}; }
std::vector<std::byte> DiscogsTrackSource::get_cover() {
    return track->get_image();
}
std::shared_ptr<Discogs::SearchResult> DiscogsTrackSource::get_track() const {
    return track;
}

void DiscogsTrackSource::set_title(const std::string &_title) {
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
void DiscogsTrackSource::set_cover(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Discogs Object is not mutable!");
}
