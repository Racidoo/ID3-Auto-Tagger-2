#include "Interfaces/IAlbum.h"
#include "Sources/Discogs/DiscogsReleaseSource.h"
#include "Sources/Spotify/SpotifyAlbumSource.h"

std::string IAlbum::get_id() const { return source ? source->get_id() : ""; }
std::string IAlbum::get_title() { return source ? source->get_title() : ""; }
std::vector<std::byte> IAlbum::get_cover() {
    return source ? source->get_cover() : std::vector<std::byte>{};
}
std::string IAlbum::get_artist() const {
    return source ? source->get_artist() : "";
}
std::string IAlbum::get_year() const {
    return source ? source->get_year() : "";
}

void IAlbum::set_title(const std::string &_title) {
    if (source)
        source->set_title(_title);
}
void IAlbum::set_cover(const std::vector<std::byte> &_imageData) {
    if (source)
        source->set_cover(_imageData);
}

std::shared_ptr<Spotify::Album> IAlbum::get_spotifyAlbum() const {
    auto src = std::dynamic_pointer_cast<SpotifyAlbumSource>(source);
    return src ? src->get_album() : nullptr;
}
std::shared_ptr<Discogs::Release> IAlbum::get_discogsRelease() const {
    auto src = std::dynamic_pointer_cast<DiscogsReleaseSource>(source);
    return src ? src->get_release() : nullptr;
}

std::shared_ptr<IAlbum>
IAlbum::fromSpotify(std::shared_ptr<Spotify::Album> _album) {
    return std::make_shared<IAlbum>(
        std::make_shared<SpotifyAlbumSource>(_album));
}

std::shared_ptr<IAlbum>
IAlbum::fromDiscogs(std::shared_ptr<Discogs::Release> _release) {
    return std::make_shared<IAlbum>(
        std::make_shared<DiscogsReleaseSource>(_release));
}
