#include "Sources/Spotify/SpotifyArtistSource.h"

const std::string &SpotifyArtistSource::get_id() const {
    return artist->get_id();
}
const std::string &SpotifyArtistSource::get_name() {
    return artist->get_name();
}
const std::vector<std::byte> &SpotifyArtistSource::get_image() {
    return artist->get_image();
}
IMediaEntity::State SpotifyArtistSource::get_state() const {
    return artist->get_state();
}

void SpotifyArtistSource::set_name(const std::string &_name) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyArtistSource::set_image(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyArtistSource::set_state(IMediaEntity::State _state) {
    artist->set_state(_state);
}

std::shared_ptr<Spotify::Artist> SpotifyArtistSource::get_artist() const {
    return artist;
}

void SpotifyArtistSource::ensureLoaded(IMediaService &_service) {
    if (artist->get_state() == IMediaEntity::State::Full)
        return;

    _service.load(*artist);
    artist->set_state(IMediaEntity::State::Full);
}