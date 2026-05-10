#include "Sources/Discogs/DiscogsArtistSource.h"

std::string DiscogsArtistSource::get_id() const { return artist->get_id(); }
std::string DiscogsArtistSource::get_title() { return artist->get_name(); }
std::vector<std::byte> DiscogsArtistSource::get_cover() {
    return artist->get_image();
}

void DiscogsArtistSource::set_title(const std::string &_title) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsArtistSource::set_cover(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Discogs Object is not mutable!");
}

std::shared_ptr<Discogs::Artist> DiscogsArtistSource::get_artist() const {
    return artist;
}