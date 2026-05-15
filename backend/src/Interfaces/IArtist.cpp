#include "Interfaces/IArtist.h"
#include "IMediaService.hpp"
#include "Sources/Discogs/DiscogsArtistSource.h"
#include "Sources/IArtistSource.hpp"
#include "Sources/Spotify/SpotifyArtistSource.h"

const std::string &IArtist::get_id() const { return source->get_id(); }
const std::string &IArtist::get_name() const { return source->get_name(); }
const std::vector<std::byte> &IArtist::get_image() {
    return source->get_image();
}

void IArtist::ensureLoaded(class IMediaService &_service) {
    source->ensureLoaded(_service);
}

std::shared_ptr<IArtist>
IArtist::fromSpotify(std::shared_ptr<Spotify::Artist> _artist) {
    return std::make_shared<IArtist>(
        std::make_shared<SpotifyArtistSource>(_artist));
}
std::shared_ptr<IArtist>
IArtist::fromDiscogs(std::shared_ptr<Discogs::Artist> _artist) {
    return std::make_shared<IArtist>(
        std::make_shared<DiscogsArtistSource>(_artist));
}