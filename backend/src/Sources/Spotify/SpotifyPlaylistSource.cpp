#include "Sources/Spotify/SpotifyPlaylistSource.h"

const std::string &SpotifyPlaylistSource::get_id() const {
    return playlist->get_id();
}
const std::string &SpotifyPlaylistSource::get_name() {
    return playlist->get_name();
}
const std::vector<std::byte> &SpotifyPlaylistSource::get_image() {
    return playlist->get_image();
}
IMediaEntity::State SpotifyPlaylistSource::get_state() const {
    return playlist->get_state();
}

void SpotifyPlaylistSource::set_name(const std::string &_name) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyPlaylistSource::set_image(
    const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyPlaylistSource::set_state(IMediaEntity::State _state) {
    playlist->set_state(_state);
}

std::shared_ptr<Spotify::Playlist> SpotifyPlaylistSource::get_playlist() const {
    return playlist;
}

void SpotifyPlaylistSource::ensureLoaded(IMediaService &_service) {
    if (playlist->get_state() == IMediaEntity::State::Full)
        return;

    _service.load(*playlist);
    playlist->set_state(IMediaEntity::State::Full);
}