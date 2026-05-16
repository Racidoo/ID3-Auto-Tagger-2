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

void ITrack::applyDifferences(std::shared_ptr<ITrack> _template) {
    if (_template->get_name() != get_name()) {
        std::cout << "Changed title from '" << get_name() << "' to '"
                  << _template->get_name() << "'" << std::endl;
        set_name(_template->get_name());
    }
    if (_template->get_artist() != get_artist()) {
        std::cout << "Changed artist from '" << get_artist() << "' to '"
                  << _template->get_artist() << "'" << std::endl;
        set_artist(_template->get_artist());
    }
    if (_template->get_albumName() != get_albumName()) {
        std::cout << "Changed album from '" << get_albumName() << "' to '"
                  << _template->get_albumName() << "'" << std::endl;
        set_albumName(_template->get_albumName());
        // replace any old cover, if album changes
        set_image(_template->get_image());
    }
    if (_template->get_albumArtist() != get_albumArtist()) {
        std::cout << "Changed albumArtist from '" << get_albumArtist()
                  << "' to '" << _template->get_albumArtist() << "'"
                  << std::endl;
        set_albumArtist(_template->get_albumArtist());
    }
    if (_template->get_copyright() != get_copyright()) {
        std::cout << "Changed copyright from '" << get_copyright() << "' to '"
                  << _template->get_copyright() << "'" << std::endl;
        set_copyright(_template->get_copyright());
    }
    // Only set genre, if it's emtpy
    if (get_genre().empty() && _template->get_genre() != get_genre()) {
        std::cout << "Changed genre from '" << get_genre() << "' to  '"
                  << _template->get_genre() << "'" << std::endl;
        set_genre(_template->get_genre());
    }
    if (_template->get_year() != get_year()) {
        std::cout << "Changed year from '" << get_year() << "' to '"
                  << _template->get_year() << "'" << std::endl;
        set_year(_template->get_year());
    }
    if (_template->get_label() != get_label()) {
        std::cout << "Changed publisher from '" << get_label() << "' to '"
                  << _template->get_label() << "'" << std::endl;
        set_label(_template->get_label());
    }
    if (_template->get_trackNumber() != get_trackNumber()) {
        std::cout << "Changed trackNumber from '" << get_trackNumber()
                  << "' to '" << _template->get_trackNumber() << "'"
                  << std::endl;
        set_trackNumber(_template->get_trackNumber());
    }
    if (_template->get_discNumber() != get_discNumber()) {
        std::cout << "Changed discNumber from '" << get_discNumber() << "' to '"
                  << _template->get_discNumber() << "'" << std::endl;
        set_discNumber(_template->get_discNumber());
    }
    if (get_image().empty()) {
        std::cout << "Applied album cover" << std::endl;
        set_image(_template->get_image());
    }
}