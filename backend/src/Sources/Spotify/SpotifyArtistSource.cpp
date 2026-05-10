#include "Sources/Spotify/SpotifyArtistSource.h"

std::string SpotifyArtistSource::get_id() const { return artist->get_id(); }
std::string SpotifyArtistSource::get_title() { return artist->get_name(); }
std::vector<std::byte> SpotifyArtistSource::get_cover() {
    return artist->get_image();
}

void SpotifyArtistSource::set_title(const std::string &_title) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyArtistSource::set_cover(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Spotify Object is not mutable!");
}

std::shared_ptr<Spotify::Artist> SpotifyArtistSource::get_artist() const {
    return artist;
}