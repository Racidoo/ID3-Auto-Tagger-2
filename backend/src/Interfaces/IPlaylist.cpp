#include "Interfaces/IPlaylist.h"
#include "IMediaService.hpp"
#include "Sources/IPlaylistSource.hpp"
#include "Sources/Spotify/SpotifyPlaylistSource.h"

const std::string &IPlaylist::get_id() const { return source->get_id(); }
const std::string &IPlaylist::get_name() const { return source->get_name(); }
const std::vector<std::byte> &IPlaylist::get_image() {
    return source->get_image();
}

void IPlaylist::ensureLoaded(class IMediaService &_service) {
    source->ensureLoaded(_service);
}

std::shared_ptr<IPlaylist>
IPlaylist::fromSpotify(std::shared_ptr<Spotify::Playlist> _track) {
    return std::make_shared<IPlaylist>(
        std::make_shared<SpotifyPlaylistSource>(_track));
}