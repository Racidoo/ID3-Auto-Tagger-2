#include "Interfaces/IAlbum.h"
// #include "Interfaces/ITrack.h"
// #include "Services/IMediaService.hpp"
// #include "Sources/Discogs/DiscogsReleaseSource.h"
// #include "Sources/Spotify/SpotifyAlbumSource.h"

// const std::string &IAlbum::get_id() const { return source->get_id(); }
// const std::string &IAlbum::get_name() const { return source->get_name(); }
// const std::vector<std::byte> &IAlbum::get_image() {
//     return source->get_image();
// }
// std::string IAlbum::get_artist() const { return source->get_artist(); }
// std::string IAlbum::get_year() const { return source->get_year(); }
// std::string IAlbum::get_type() const { return source->get_type(); }
// std::vector<std::byte> IAlbum::get_artistCover() const {
//     return source->get_artistCover();
// }
// std::vector<std::shared_ptr<ITrack>> IAlbum::get_tracklist() const {
//     return source->get_tracklist();
// }

// void IAlbum::ensureLoaded(class IMediaService &_service) {
//     source->ensureLoaded(_service);
// }

// std::shared_ptr<Discogs::Release> IAlbum::get_discogsRelease() const {
//     auto src = std::dynamic_pointer_cast<DiscogsReleaseSource>(source);
//     return src ? src->get_release() : nullptr;
// }

// std::shared_ptr<IAlbum>
// IAlbum::fromSpotify(std::shared_ptr<Spotify::Album> _album) {
//     return std::make_shared<IAlbum>(
//         std::make_shared<SpotifyAlbumSource>(_album));
// }

// std::shared_ptr<IAlbum>
// IAlbum::fromDiscogs(std::shared_ptr<Discogs::Release> _release) {
//     return std::make_shared<IAlbum>(
//         std::make_shared<DiscogsReleaseSource>(_release));
// }
