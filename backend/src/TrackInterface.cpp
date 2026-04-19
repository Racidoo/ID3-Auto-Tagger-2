#include "../include/TrackInterface.h"
#include "../include/Downloader.h"
#include "../include/Query.h"

std::string TrackInterface::get_id() const {
    if (local)
        return local->get_filename();
    if (spotify)
        return spotify->get_id();
    assert(!local && !spotify && "Can not generate a valid id");
    return {};
}

std::string TrackInterface::get_title() {
    if (local)
        return local->get_title();
    if (spotify)
        return spotify->get_name();
    return {};
}
std::string TrackInterface::get_artist() {
    if (local)
        return local->get_artist();
    if (spotify)
        return spotify->get_stringArtists();
    return {};
}
std::string TrackInterface::get_album() {
    if (local)
        return local->get_album();
    if (spotify)
        return spotify->get_album().get_name();
    return {};
}
std::string TrackInterface::get_albumArtist() {
    if (local)
        return local->get_albumArtist();
    if (spotify)
        return spotify->get_album().get_artists();
    return {};
}
std::string TrackInterface::get_copyright() {
    if (local)
        return local->get_copyright();
    if (spotify)
        return spotify->get_album().get_copyright();
    return {};
}
std::string TrackInterface::get_genre() {
    if (local)
        return local->get_genre();
    // if (spotify)
    //     return spotify->get_genre();
    return {};
}
std::string TrackInterface::get_year() {
    if (local)
        return local->get_year();
    if (spotify)
        return spotify->get_album().get_releaseYear();
    return {};
}
std::string TrackInterface::get_label() {
    if (local)
        return local->get_label();
    if (spotify)
        return spotify->get_album().get_label();
    return {};
}
std::string TrackInterface::get_track() {
    if (local)
        return local->get_track();
    if (spotify)
        return std::to_string(spotify->get_trackNumber());
    return {};
}
std::string TrackInterface::get_disc() {
    if (local)
        return local->get_disc();
    if (spotify)
        return std::to_string(spotify->get_discNumber());
    return {};
}
std::size_t TrackInterface::get_length() {
    if (local)
        return local->get_length();
    if (spotify)
        return spotify->get_durationMs() / 1000;
    return {};
}

std::vector<std::byte> TrackInterface::get_cover() {
    if (local)
        return local->get_cover();
    if (spotify)
        return spotify->get_album().get_image();
    return {};
}

const std::shared_ptr<LocalTrack> TrackInterface::get_localTrack() const {
    return local;
}
const std::shared_ptr<Spotify::Track> TrackInterface::get_spotifyTrack() const {
    return spotify;
}

bool TrackInterface::is_inBlocklist() const { return inBlocklist; }
bool TrackInterface::is_verified() const { return verified; }
bool TrackInterface::is_downloaded() const { return downloaded; }
bool TrackInterface::is_localTrack() const { return local != nullptr; }
bool TrackInterface::is_spotifyTrack() const { return spotify != nullptr; }

void TrackInterface::set_title(const std::string &_title) {
    if (local) {
        local->set_title(_title);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_artist(const std::string &_artist) {
    if (local) {
        local->set_artist(_artist);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_album(const std::string &_album) {
    if (local) {
        local->set_album(_album);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_albumArtist(const std::string &_albumArtist) {
    if (local) {
        local->set_albumArtist(_albumArtist);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_copyright(const std::string &_copyright) {
    if (local) {
        local->set_copyright(_copyright);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_genre(const std::string &_genre) {
    if (local) {
        local->set_genre(_genre);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_year(const std::string &_year) {
    if (local) {
        local->set_year(_year);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_label(const std::string &_label) {
    if (local) {
        local->set_label(_label);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_track(const std::string &_track) {
    if (local) {
        local->set_track(_track);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_disc(const std::string &_disc) {
    if (local) {
        local->set_disc(_disc);
        return;
    }
    std::cerr << "Warning: No local file provided for " << get_id()
              << std::endl;
}

void TrackInterface::set_cover(const std::vector<std::byte> &_imageData) {
    if (local) {
        local->set_cover(_imageData);
    }
}

void TrackInterface::set_inBlocklist(bool _inBlocklist) {
    inBlocklist = _inBlocklist;
}
void TrackInterface::set_verified(bool _verified) { verified = _verified; }
void TrackInterface::set_downloaded(bool _downloaded) {
    downloaded = _downloaded;
}

std::shared_ptr<TrackInterface>
TrackInterface::fromLocal(const LocalTrack &_track) {
    TrackInterface v;
    v.local = std::make_shared<LocalTrack>(_track);
    v.downloaded = true;

    return std::make_shared<TrackInterface>(v);
}

std::shared_ptr<TrackInterface>
TrackInterface::fromSpotify(const Spotify::Track &_track) {
    TrackInterface v;
    v.verified = true;
    v.downloaded = _track.isDownloaded();
    v.spotify = std::make_shared<Spotify::Track>(_track);
    return std::make_shared<TrackInterface>(v);
}

void TrackInterface::verify(std::shared_ptr<TrackInterface> _data,
                            Downloader *_downloader) {

    if (!_data || !_downloader) {
        return;
    }

    const auto &filepath = _data->local->get_filepath();

    auto spotifyTrackData = _downloader->get_spotify()->researchTags(_data);
    if (!spotifyTrackData || !spotifyTrackData->spotify) {
        std::cerr
            << "Unable to retrieve meta data from SpotifyAPI. Will not add '"
            << filepath << "' to blacklist!" << std::endl;
        return;
    }
    _data->verifyTags(spotifyTrackData);
    const auto &id = spotifyTrackData->spotify->get_id();
    if (filepath.stem() != id) {
        std::filesystem::path newPath =
            filepath.parent_path() / (id + filepath.extension().string());
        try {
            std::filesystem::rename(filepath, newPath);
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "Rename failed: " << e.what() << std::endl;
            return;
        }
        _data->local->set_filepath(newPath);
    }
    _downloader->makeBlocked(_data);
    _data->inBlocklist = true;
}

void TrackInterface::verifyTags(std::shared_ptr<TrackInterface> _template) {

    assert(_template->spotify && this->local && "Invalid verify assignment");

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
    if (_template->get_track() != get_track()) {
        std::cout << "Changed trackNumber from '" << get_track() << "' to '"
                  << _template->get_track() << "'" << std::endl;
        set_track(_template->get_track());
    }
    if (_template->get_disc() != get_disc()) {
        std::cout << "Changed discNumber from '" << get_disc() << "' to '"
                  << _template->get_disc() << "'" << std::endl;
        set_disc(_template->get_disc());
    }
}