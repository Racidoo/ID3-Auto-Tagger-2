#include "Sources/Discogs/DiscogsMasterReleaseSource.h"

std::string DiscogsMasterReleaseSource::get_id() const {
    return release->get_id();
}
std::string DiscogsMasterReleaseSource::get_title() {
    return release->get_name();
}
std::vector<std::byte> DiscogsMasterReleaseSource::get_cover() {
    return release->get_image();
}
std::string DiscogsMasterReleaseSource::get_artist() {
    return release->get_stringArtists();
}
std::string DiscogsMasterReleaseSource::get_copyright() { return {}; }
std::string DiscogsMasterReleaseSource::get_genre() {
    return release->get_stringGenres();
}
std::string DiscogsMasterReleaseSource::get_year() {
    return std::to_string(release->get_year());
}
std::string DiscogsMasterReleaseSource::get_label() { return {}; }

void DiscogsMasterReleaseSource::set_title(const std::string &_title) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsMasterReleaseSource::set_cover(
    const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsMasterReleaseSource::set_artist(const std::string &_artist) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsMasterReleaseSource::set_copyright(const std::string &_copyright) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsMasterReleaseSource::set_genre(const std::string &_genre) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsMasterReleaseSource::set_year(const std::string &_year) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsMasterReleaseSource::set_label(const std::string &_label) {
    throw std::logic_error("Discogs Object is not mutable!");
}
std::shared_ptr<Discogs::MasterRelease>
DiscogsMasterReleaseSource::get_release() const {
    return release;
}