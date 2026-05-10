#include "Sources/Spotify/SpotifyPlaylistSource.h"

std::string SpotifyPlaylistSource::get_id() const { return playlist->get_id(); }
std::string SpotifyPlaylistSource::get_title() { return playlist->get_name(); }
std::vector<std::byte> SpotifyPlaylistSource::get_cover() {
    return playlist->get_image();
}

void SpotifyPlaylistSource::set_title(const std::string &_title) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyPlaylistSource::set_cover(
    const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Spotify Object is not mutable!");
}

std::shared_ptr<Spotify::Playlist> SpotifyPlaylistSource::get_playlist() const {
    return playlist;
}