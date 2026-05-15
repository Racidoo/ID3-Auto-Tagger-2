#include "Sources/Discogs/DiscogsArtistSource.h"

const std::string &DiscogsArtistSource::get_id() const {
    return artist->get_id();
}
const std::string &DiscogsArtistSource::get_name() {
    return artist->get_name();
}
const std::vector<std::byte> &DiscogsArtistSource::get_image() {
    return artist->get_image();
}
IMediaEntity::State DiscogsArtistSource::get_state() const {
    return artist->get_state();
}

void DiscogsArtistSource::set_name(const std::string &_name) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsArtistSource::set_image(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Discogs Object is not mutable!");
}
void DiscogsArtistSource::set_state(IMediaEntity::State _state) {
    artist->set_state(_state);
}

std::shared_ptr<Discogs::Artist> DiscogsArtistSource::get_artist() const {
    return artist;
}

void DiscogsArtistSource::ensureLoaded(IMediaService &_service) {
    if (artist->get_state() == IMediaEntity::State::Full)
        return;

    _service.load(*artist);
    artist->set_state(IMediaEntity::State::Full);
}