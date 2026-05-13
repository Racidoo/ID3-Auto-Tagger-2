#include "Sources/Spotify/SpotifyAlbumSource.h"
#include "Interfaces/ITrack.h"
#include "Spotify/Album.h"
#include "Spotify/Track.h"

std::string SpotifyAlbumSource::get_id() const { return album->get_id(); }
std::string SpotifyAlbumSource::get_title() { return album->get_name(); }
std::vector<std::byte> SpotifyAlbumSource::get_cover() {
    return album->get_image();
}
std::string SpotifyAlbumSource::get_artist() {
    return album->get_stringArtists();
}
std::string SpotifyAlbumSource::get_copyright() {
    return album->get_copyright();
}
std::string SpotifyAlbumSource::get_genre() { return {}; }
std::string SpotifyAlbumSource::get_year() { return album->get_releaseYear(); }
std::string SpotifyAlbumSource::get_label() { return album->get_label(); }
std::string SpotifyAlbumSource::get_type() const {
    return album->get_albumType();
}
std::vector<std::byte> SpotifyAlbumSource::get_artistCover() const {
    return (album->get_artists().empty() ? std::vector<std::byte>{}
                                         : album->get_artists()[0].get_image());
}
std::vector<std::shared_ptr<ITrack>> SpotifyAlbumSource::get_tracklist() const {
    std::vector<std::shared_ptr<ITrack>> tracklist;
    for (auto &&track : album->get_tracklist()) {
        tracklist.push_back(
            ITrack::fromSpotify(std::make_shared<Spotify::Track>(track)));
    }
    return tracklist;
}
bool SpotifyAlbumSource::isMetaDataComplete() const {
    return album->get_state() == Spotify::Album::MetadataState::Full;
}

void SpotifyAlbumSource::set_title(const std::string &_title) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyAlbumSource::set_cover(const std::vector<std::byte> &_imageData) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyAlbumSource::set_artist(const std::string &_artist) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyAlbumSource::set_copyright(const std::string &_copyright) {
    album->set_copyright(_copyright);
}
void SpotifyAlbumSource::set_genre(const std::string &_genre) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyAlbumSource::set_year(const std::string &_year) {
    throw std::logic_error("Spotify Object is not mutable!");
}
void SpotifyAlbumSource::set_label(const std::string &_label) {
    album->set_label(_label);
}
std::shared_ptr<Spotify::Album> SpotifyAlbumSource::get_album() const {
    return album;
}