#include "Interfaces/IArtist.h"
#include "Sources/Discogs/DiscogsArtistSource.h"
#include "Sources/IArtistSource.hpp"
#include "Sources/Spotify/SpotifyArtistSource.h"

std::string IArtist::get_id() const { return source ? source->get_id() : ""; }
std::string IArtist::get_title() { return source ? source->get_title() : ""; }
std::vector<std::byte> IArtist::get_cover() {
    return source ? source->get_cover() : std::vector<std::byte>{};
}

void IArtist::set_title(const std::string &_title) {
    if (source)
        source->set_title(_title);
}
void IArtist::set_cover(const std::vector<std::byte> &_imageData) {
    if (source)
        source->set_cover(_imageData);
}

std::shared_ptr<IArtist>
IArtist::fromSpotify(std::shared_ptr<Spotify::Artist> _track) {
    return std::make_shared<IArtist>(
        std::make_shared<SpotifyArtistSource>(_track));
}
std::shared_ptr<IArtist>
IArtist::fromDiscogs(std::shared_ptr<Discogs::Artist> _track) {
    return std::make_shared<IArtist>(
        std::make_shared<DiscogsArtistSource>(_track));
}