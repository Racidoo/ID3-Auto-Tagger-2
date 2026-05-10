#include "Interfaces/IPlaylist.h"
#include "Sources/IPlaylistSource.hpp"
#include "Sources/Spotify/SpotifyPlaylistSource.h"

std::string IPlaylist::get_id() const { return source ? source->get_id() : ""; }
std::string IPlaylist::get_title() { return source ? source->get_title() : ""; }
std::vector<std::byte> IPlaylist::get_cover() {
    return source ? source->get_cover() : std::vector<std::byte>{};
}

void IPlaylist::set_title(const std::string &_title) {
    if (source)
        source->set_title(_title);
}
void IPlaylist::set_cover(const std::vector<std::byte> &_imageData) {
    if (source)
        source->set_cover(_imageData);
}

std::shared_ptr<IPlaylist>
IPlaylist::fromSpotify(std::shared_ptr<Spotify::Playlist> _track) {
    return std::make_shared<IPlaylist>(
        std::make_shared<SpotifyPlaylistSource>(_track));
}