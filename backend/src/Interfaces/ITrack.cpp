#include "Interfaces/ITrack.h"
#include "Discogs/ReleaseTrack.h"
#include "Downloader.h"
#include "LocalTrack.h"
#include "Query.h"
#include "Sources/Discogs/DiscogsTrackSource.h"
#include "Sources/ITrackSource.hpp"
#include "Sources/LocalTrackSource.h"
#include "Sources/Spotify/SpotifyTrackSource.h"
#include "Spotify/Track.h"

std::string ITrack::get_id() const { return source ? source->get_id() : ""; }
std::string ITrack::get_title() { return source ? source->get_title() : ""; }
std::string ITrack::get_artist() { return source ? source->get_artist() : ""; }
std::string ITrack::get_album() { return source ? source->get_album() : ""; }
std::string ITrack::get_albumArtist() {
    return source ? source->get_albumArtist() : "";
}
std::string ITrack::get_copyright() {
    return source ? source->get_copyright() : "";
}
std::string ITrack::get_genre() { return source ? source->get_genre() : ""; }
std::string ITrack::get_year() { return source ? source->get_year() : ""; }
std::string ITrack::get_label() { return source ? source->get_label() : ""; }
std::string ITrack::get_trackNumber() {
    return source ? source->get_trackNumber() : "";
}
std::string ITrack::get_discNumber() {
    return source ? source->get_discNumber() : "";
}
std::size_t ITrack::get_length() { return source ? source->get_length() : 0; }
std::vector<std::byte> ITrack::get_cover() {
    return source ? source->get_cover() : std::vector<std::byte>{};
}

bool ITrack::is_verified() const { return verified; }

void ITrack::set_title(const std::string &_title) {
    if (source)
        source->set_title(_title);
}
void ITrack::set_artist(const std::string &_artist) {
    if (source)
        source->set_artist(_artist);
}
void ITrack::set_album(const std::string &_album) {
    if (source)
        source->set_album(_album);
}
void ITrack::set_albumArtist(const std::string &_albumArtist) {
    if (source)
        source->set_albumArtist(_albumArtist);
}
void ITrack::set_copyright(const std::string &_copyright) {
    if (source)
        source->set_copyright(_copyright);
}
void ITrack::set_genre(const std::string &_genre) {
    if (source)
        source->set_genre(_genre);
}
void ITrack::set_year(const std::string &_year) {
    if (source)
        source->set_year(_year);
}
void ITrack::set_label(const std::string &_label) {
    if (source)
        source->set_label(_label);
}
void ITrack::set_trackNumber(const std::string &_track) {
    if (source)
        source->set_trackNumber(_track);
}
void ITrack::set_discNumber(const std::string &_disc) {
    if (source)
        source->set_discNumber(_disc);
}
void ITrack::set_cover(const std::vector<std::byte> &_imageData) {
    if (source)
        source->set_cover(_imageData);
}

void ITrack::set_verified(bool _verified) { verified = _verified; }

std::shared_ptr<Spotify::Track> ITrack::get_spotifyTrack() const {
    auto src = std::dynamic_pointer_cast<SpotifyTrackSource>(source);
    return src ? src->get_track() : nullptr;
}

std::shared_ptr<LocalTrack> ITrack::get_localTrack() const {
    auto src = std::dynamic_pointer_cast<LocalTrackSource>(source);
    return src ? src->get_track() : nullptr;
}

std::shared_ptr<Discogs::ReleaseTrack> ITrack::get_discogsTrack() const {
    auto src = std::dynamic_pointer_cast<DiscogsTrackSource>(source);
    return src ? src->get_track() : nullptr;
}

std::shared_ptr<ITrack> ITrack::fromLocal(std::shared_ptr<LocalTrack> _track) {
    auto trackInterface =
        std::make_shared<ITrack>(std::make_shared<LocalTrackSource>(_track));
    return trackInterface;
}

std::shared_ptr<ITrack>
ITrack::fromSpotify(std::shared_ptr<Spotify::Track> _track) {
    auto trackInterface =
        std::make_shared<ITrack>(std::make_shared<SpotifyTrackSource>(_track));
    trackInterface->set_verified(_track->isDownloaded());
    return trackInterface;
}

std::shared_ptr<ITrack>
ITrack::fromDiscogs(std::shared_ptr<Discogs::ReleaseTrack> _track) {
    auto trackInterface =
        std::make_shared<ITrack>(std::make_shared<DiscogsTrackSource>(_track));
    trackInterface->set_verified(false);
    return trackInterface;
}

bool ITrack::verify(std::shared_ptr<ITrack> _data, Downloader *_downloader) {

    if (!_data || !_downloader || !_data->get_localTrack()) {
        return false;
    }

    const auto &filepath = _data->get_localTrack()->get_filepath();

    auto spotifyTrackData = _downloader->get_spotify()->researchTags(_data);
    if (!spotifyTrackData || !spotifyTrackData->get_spotifyTrack()) {
        std::cerr
            << "Unable to retrieve meta data from SpotifyAPI. Will not add '"
            << filepath << "' to blacklist!" << std::endl;
        return false;
    }

    if (!_downloader->initializeDiscogs()) {
        Discogs::DiscogsAPI::SearchParams params{};

        params.releaseTitle = spotifyTrackData->get_album();
        params.artist = spotifyTrackData->get_albumArtist();
        params.year = std::stoi(spotifyTrackData->get_year());

        auto releases = _downloader->get_discogs()->searchRelease(params);
        if (releases.empty()) {
            std::cerr << "Could not locate track details using DicogsAPI!"
                      << std::endl;
        }

        std::unordered_set<std::string> styles;
        for (auto &&release : releases) {
            for (auto &&style : release->get_styles()) {
                styles.insert(style);
            }
        }
        if (styles.size() == 1) {
            spotifyTrackData->set_genre(*styles.begin());
        } else {
            std::cout << "Cannot determine correct genre:" << std::endl;
            for (auto &&style : styles) {
                std::cout << style << std::endl;
            }
        }
    } else {
        std::cerr << "DiscogsAPI unavailable. Cannot determine genre!"
                  << std::endl;
    }

    _data->verifyTags(spotifyTrackData);

    const auto &id = spotifyTrackData->get_id();
    if (filepath.stem() != id) {
        _downloader->removeBlocked(_data);
        _data->get_localTrack()->renameLocalTrack(id);
    }
    _downloader->makeBlocked(_data);

    return true;
}

void ITrack::verifyTags(std::shared_ptr<ITrack> _template) {

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
    if (get_cover().empty()) {
        std::cout << "Applied album cover" << std::endl;
        set_cover(_template->get_cover());
    }
}