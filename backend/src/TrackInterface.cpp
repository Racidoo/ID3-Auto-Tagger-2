#include "../include/TrackInterface.h"
#include "../include/Downloader.h"
#include "../include/Query.h"
#include "TrackSource/LocalTrackSource.h"
#include "TrackSource/SpotifyTrackSource.h"

std::string TrackInterface::get_id() const {
    return source ? source->get_id() : "";
}
std::string TrackInterface::get_title() {
    return source ? source->get_title() : "";
}
std::string TrackInterface::get_artist() {
    return source ? source->get_artist() : "";
}
std::string TrackInterface::get_album() {
    return source ? source->get_album() : "";
}
std::string TrackInterface::get_albumArtist() {
    return source ? source->get_albumArtist() : "";
}
std::string TrackInterface::get_copyright() {
    return source ? source->get_copyright() : "";
}
std::string TrackInterface::get_genre() {
    return source ? source->get_genre() : "";
}
std::string TrackInterface::get_year() {
    return source ? source->get_year() : "";
}
std::string TrackInterface::get_label() {
    return source ? source->get_label() : "";
}
std::string TrackInterface::get_trackNumber() {
    return source ? source->get_trackNumber() : "";
}
std::string TrackInterface::get_discNumber() {
    return source ? source->get_discNumber() : "";
}
std::size_t TrackInterface::get_length() {
    return source ? source->get_length() : 0;
}
std::vector<std::byte> TrackInterface::get_cover() {
    return source ? source->get_cover() : std::vector<std::byte>{};
}

bool TrackInterface::is_inBlocklist() const { return inBlocklist; }
bool TrackInterface::is_verified() const { return verified; }
bool TrackInterface::is_downloaded() const { return downloaded; }

void TrackInterface::set_title(const std::string &_title) {
    if (source)
        source->set_title(_title);
}
void TrackInterface::set_artist(const std::string &_artist) {
    if (source)
        source->set_artist(_artist);
}
void TrackInterface::set_album(const std::string &_album) {
    if (source)
        source->set_album(_album);
}
void TrackInterface::set_albumArtist(const std::string &_albumArtist) {
    if (source)
        source->set_albumArtist(_albumArtist);
}
void TrackInterface::set_copyright(const std::string &_copyright) {
    if (source)
        source->set_copyright(_copyright);
}
void TrackInterface::set_genre(const std::string &_genre) {
    if (source)
        source->set_genre(_genre);
}
void TrackInterface::set_year(const std::string &_year) {
    if (source)
        source->set_year(_year);
}
void TrackInterface::set_label(const std::string &_label) {
    if (source)
        source->set_label(_label);
}
void TrackInterface::set_trackNumber(const std::string &_track) {
    if (source)
        source->set_trackNumber(_track);
}
void TrackInterface::set_discNumber(const std::string &_disc) {
    if (source)
        source->set_discNumber(_disc);
}
void TrackInterface::set_cover(const std::vector<std::byte> &_imageData) {
    if (source)
        source->set_cover(_imageData);
}

void TrackInterface::set_inBlocklist(bool _inBlocklist) {
    inBlocklist = _inBlocklist;
}
void TrackInterface::set_verified(bool _verified) { verified = _verified; }
void TrackInterface::set_downloaded(bool _downloaded) {
    downloaded = _downloaded;
}

std::shared_ptr<Spotify::Track> TrackInterface::get_spotifyTrack() const {
    auto src = std::dynamic_pointer_cast<SpotifyTrackSource>(source);
    return src ? src->get_track() : nullptr;
}

std::shared_ptr<LocalTrack> TrackInterface::get_localTrack() const {
    auto src = std::dynamic_pointer_cast<LocalTrackSource>(source);
    return src ? src->get_track() : nullptr;
}

std::shared_ptr<TrackInterface>
TrackInterface::fromLocal(std::shared_ptr<LocalTrack> _track) {
    auto trackInterface = std::make_shared<TrackInterface>(
        std::make_shared<LocalTrackSource>(_track));
    trackInterface->set_downloaded(true);
    return trackInterface;
}

std::shared_ptr<TrackInterface>
TrackInterface::fromSpotify(std::shared_ptr<Spotify::Track> _track) {
    auto trackInterface = std::make_shared<TrackInterface>(
        std::make_shared<SpotifyTrackSource>(_track));
    trackInterface->set_verified(true);
    trackInterface->set_downloaded(_track->isDownloaded());
    return trackInterface;
}

void TrackInterface::verify(std::shared_ptr<TrackInterface> _data,
                            Downloader *_downloader) {

    if (!_data || !_downloader || !_data->get_localTrack()) {
        return;
    }

    const auto &filepath = _data->get_localTrack()->get_filepath();

    auto spotifyTrackData = _downloader->get_spotify()->researchTags(_data);
    if (!spotifyTrackData || !spotifyTrackData->get_spotifyTrack()) {
        std::cerr
            << "Unable to retrieve meta data from SpotifyAPI. Will not add '"
            << filepath << "' to blacklist!" << std::endl;
        return;
    }
    _data->verifyTags(spotifyTrackData);
    const auto &id = spotifyTrackData->get_id();
    if (filepath.stem() != id) {
        _downloader->removeBlocked(_data);
        _data->get_localTrack()->renameLocalTrack(id);
    }
    _downloader->makeBlocked(_data);
    _data->inBlocklist = true;
}

void TrackInterface::verifyTags(std::shared_ptr<TrackInterface> _template) {

    assert(_template->get_spotifyTrack() && this->get_localTrack() &&
           "Invalid verify assignment");

    if (_template->get_title() != get_title()) {
        std::cout << "Changed title from '" << get_title() << "' to '"
                  << _template->get_title() << "'" << std::endl;
        set_title(_template->get_title());
    }
    if (_template->get_artist() != get_artist()) {
        std::cout << "Changed artist from '" << get_artist() << "' to '"
                  << _template->get_artist() << "'" << std::endl;
        set_artist(_template->get_artist());
    }
    if (_template->get_album() != get_album()) {
        std::cout << "Changed album from '" << get_album() << "' to '"
                  << _template->get_album() << "'" << std::endl;
        set_album(_template->get_album());
        // replace any old cover, if album changes
        set_cover(_template->get_cover());
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
    // skip genre as it's not correctly provided
    // if (_template->get_genre() != get_genre()) {
    //     set_genre(_template->get_genre());
    // }
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
    if (get_cover().empty()) {
        std::cout << "Applied album cover" << std::endl;
        set_cover(_template->get_cover());
    }
}