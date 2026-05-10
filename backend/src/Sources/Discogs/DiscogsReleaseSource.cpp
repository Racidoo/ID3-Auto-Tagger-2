#include "Sources/Discogs/DiscogsReleaseSource.h"

std::string DiscogsReleaseSource::get_id() const { return release->get_id(); }
std::string DiscogsReleaseSource::get_title() { return release->get_name(); }
std::vector<std::byte> DiscogsReleaseSource::get_cover() {
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

void DiscogsReleaseSource::set_title(const std::string &_title) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsReleaseSource::set_cover(const std::vector<std::byte> &_imageData) {
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
std::shared_ptr<Discogs::Release> DiscogsReleaseSource::get_release() const {
    return release;
}