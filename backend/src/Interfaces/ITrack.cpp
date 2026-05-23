#include "Interfaces/ITrack.h"
// #include "Discogs/ReleaseTrack.h"
// #include "Downloader.h"
// #include "Local/LocalTrack.h"
// #include "Query.h"
// #include "Sources/Discogs/DiscogsTrackSource.h"
// #include "Sources/LocalTrackSource.h"
// #include "Sources/Spotify/SpotifyTrackSource.h"
// #include "Spotify/Track.h"

// const std::string &ITrack::get_id() const { return source->get_id(); }
// const std::string &ITrack::get_name() const { return source->get_name(); }
// std::string ITrack::get_artist() { return source->get_artist(); }
// std::string ITrack::get_album() { return source->get_album(); }
// std::string ITrack::get_albumArtist() { return source->get_albumArtist(); }
// std::string ITrack::get_copyright() { return source->get_copyright(); }
// std::string ITrack::get_genre() { return source->get_genre(); }
// std::string ITrack::get_year() { return source->get_year(); }
// std::string ITrack::get_label() { return source->get_label(); }
// std::string ITrack::get_trackNumber() { return source->get_trackNumber(); }
// std::string ITrack::get_discNumber() { return source->get_discNumber(); }
// std::size_t ITrack::get_length() { return source->get_length(); }
// const std::vector<std::byte> &ITrack::get_image() {
//     return source->get_image();
// }

// bool ITrack::is_verified() const { return verified; }

// void ITrack::set_name(const std::string &_name) { source->set_name(_name); }
// void ITrack::set_artist(const std::string &_artist) {
//     source->set_artist(_artist);
// }
// void ITrack::set_album(const std::string &_album) { source->set_album(_album); }
// void ITrack::set_albumArtist(const std::string &_albumArtist) {
//     source->set_albumArtist(_albumArtist);
// }
// void ITrack::set_copyright(const std::string &_copyright) {
//     source->set_copyright(_copyright);
// }
// void ITrack::set_genre(const std::string &_genre) { source->set_genre(_genre); }
// void ITrack::set_year(const std::string &_year) { source->set_year(_year); }
// void ITrack::set_label(const std::string &_label) { source->set_label(_label); }
// void ITrack::set_trackNumber(const std::string &_track) {
//     source->set_trackNumber(_track);
// }
// void ITrack::set_discNumber(const std::string &_disc) {
//     source->set_discNumber(_disc);
// }
// void ITrack::set_image(const std::vector<std::byte> &_imageData) {
//     source->set_image(_imageData);
// }

// void ITrack::set_verified(bool _verified) { verified = _verified; }

// void ITrack::ensureLoaded(class IMediaService &_service) {
//     source->ensureLoaded(_service);
// }

// std::shared_ptr<ITrack> ITrack::fromLocal(std::shared_ptr<LocalTrack> _track) {
//     auto trackInterface =
//         std::make_shared<ITrack>(std::make_shared<LocalTrackSource>(_track));
//     return trackInterface;
// }

// std::shared_ptr<ITrack>
// ITrack::fromSpotify(std::shared_ptr<Spotify::Track> _track) {
//     auto trackInterface =
//         std::make_shared<ITrack>(std::make_shared<SpotifyTrackSource>(_track));
//     trackInterface->set_verified(_track->isDownloaded());
//     return trackInterface;
// }

// std::shared_ptr<ITrack>
// ITrack::fromDiscogs(std::shared_ptr<Discogs::ReleaseTrack> _track) {
//     auto trackInterface =
//         std::make_shared<ITrack>(std::make_shared<DiscogsTrackSource>(_track));
//     trackInterface->set_verified(false);
//     return trackInterface;
// }

