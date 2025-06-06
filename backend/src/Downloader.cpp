#include "../include/Downloader.h"

Downloader::Downloader()
    : trackPath(std::filesystem::current_path() / "music/"), spotify(nullptr),
      youTube(nullptr)
//   ,coverPath(std::filesystem::current_path() / "cover/")
{
    initialize();
    loadOrCreateBlacklist();
}

Downloader::~Downloader() {
    writeBlacklist();
    if (spotify)
        delete spotify;
    if (youTube)
        delete youTube;
}

bool Downloader::is_initialized() const { return (spotify && youTube); }

bool Downloader::initialize() {

    return initializeSpotify() && initializeYouTube();
}

bool Downloader::initializeSpotify(const std::string &_spotifyclientId,
                                   const std::string &_spotifyClientSecret) {
    if (!spotify) {
        Spotify::SpotifyAPI *temp =
            (_spotifyclientId.empty() && _spotifyClientSecret.empty()
                 ? new Spotify::SpotifyAPI()
                 : new Spotify::SpotifyAPI(_spotifyclientId,
                                           _spotifyClientSecret));
        if (temp->is_initialized()) {
            spotify = temp;
        } else {
            delete temp;
        }
    }
    return spotify;
}

bool Downloader::initializeYouTube(const std::string &_googleAuthToken) {
    if (!youTube) {
        std::cout << "Initializing YouTube...";
        YouTube *temp =
            (_googleAuthToken.empty() ? new YouTube()
                                      : new YouTube(_googleAuthToken));
        if (temp->is_initialized()) {
            std::cout << " success!" << std::endl;
            youTube = temp;
        } else {
            std::cout << " failed!" << std::endl;
            delete temp;
        }
    }
    return youTube;
}

bool Downloader::loadOrCreateBlacklist() {
    const std::filesystem::path path = "blacklist.json";

    if (!std::filesystem::exists(path)) {
        // Create missing blacklist.json with default object structure
        blacklist = {
            {"blacklist",
             json::array()} // "blacklist" key maps to an empty array
        };

        std::ofstream file(path);
        file << blacklist.dump(4);
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open blacklist.json!" << std::endl;
        return false;
    }

    file >> blacklist;
    return true;
}

bool Downloader::writeBlacklist() const {
    std::ofstream blacklistFile("blacklist.json");
    if (!blacklistFile.is_open()) {
        std::cerr << "Could not write blacklist.json!" << std::endl;
        return false;
    }
    blacklistFile << blacklist.dump(4);
    return true;
}

/**
 * @brief
 *
 * @param _query
 * @param categories
 * @return Downloader::SearchResult
 */
Downloader::SearchResult
Downloader::fetchResource(const std::string &_query,
                          const std::set<SearchCategory> &categories,
                          const std::string &_market, const std::string &_limit,
                          const std::string &_offset) {

    std::regex spotifyUrlPattern(
        R"(https:\/\/open\.spotify\.com\/(?:intl-[a-z]{2}\/)?(playlist|album|track)\/([\w]+))");
    // std::regex youTubeUrlPattern(
    //     R"(https:\/\/www\.youtube\.com\/watch\?v=(\w*))");

    std::string urlType;
    SearchResult result;
    std::smatch matches;
    std::string type, id;
    if (std::regex_search(_query, matches, spotifyUrlPattern)) {
        if (matches[1].matched && matches[2].matched) {
            urlType = "Spotify";
            type = matches[1].str();
            id = matches[2].str();
        }
    }
    //  else if (std::regex_search(_query, matches, youTubeUrlPattern)) {
    //     if (matches[1].matched && matches[2].matched) {
    //         urlType = "YouTube";
    //         id = matches[1].str();
    //     }
    // }

    if (type == "track") {
        auto track = spotify->getTrack(id);
        result.tracks.push_back(track);
        track.set_downloaded(isBlocked(track.get_id()));
    } else if (type == "album") {
        auto tracks = spotify->getAlbumTracks(id);
        for (auto &&track : tracks) {
            track.set_downloaded(isBlocked(track.get_id()));
            result.tracks.push_back(track);
        }
    } else if (type == "playlist") {
        auto tracks = spotify->getPlaylistTracks(id);
        for (auto &&track : tracks) {
            track.set_downloaded(isBlocked(track.get_id()));
            result.tracks.push_back(track);
        }
        // } else if (urlType == "YouTube") {
    } else {

        for (auto &&cat : categories) {
            switch (cat) {
            case SearchCategory::Track: {
                auto tracks =
                    spotify->searchTrack(_query, _market, _limit, _offset);
                for (auto &t : tracks) {
                    t.set_downloaded(isBlocked(t.get_id()));
                    result.tracks.push_back(std::move(t));
                }
                break;
            }
            case SearchCategory::Album: {
                auto albums =
                    spotify->searchAlbum(_query, _market, _limit, _offset);
                for (auto &a : albums) {
                    result.albums.push_back(std::move(a));
                }
                break;
            }
            case SearchCategory::Artist: {
                auto artists =
                    spotify->searchArtist(_query, _market, _limit, _offset);
                for (auto &a : artists) {
                    result.artists.push_back(std::move(a));
                }
                break;
            }
            case SearchCategory::Playlist: {
                auto playlists =
                    spotify->searchPlaylist(_query, _market, _limit, _offset);
                for (auto &p : playlists) {
                    result.playlists.push_back(std::move(p));
                }
                break;
            }
            }
        }
    }
    return result;
}

std::string Downloader::downloadResource(std::vector<Spotify::Track> &&_tracks,
                                         std::function<void(int)> _onProgress) {
    std::string result;
    for (auto &&track : _tracks) {
        downloadAndTag(track, _onProgress);
    }
    return result;
}

bool Downloader::isBlocked(const std::string &_id) const {
    return blacklist.at("blacklist").contains(_id);
}

void Downloader::makeBlocked(const Spotify::Track &_track) {
    blacklist["blacklist"][_track.get_id()]["title"] = _track.get_name();
    blacklist["blacklist"][_track.get_id()]["artist"] =
        _track.get_stringArtists();
}

void Downloader::downloadAndTag(Spotify::Track &_track,
                                std::function<void(int)> _onProgress) {

    std::thread([this, _onProgress, _track]() mutable {
        _onProgress(0);
        auto bestMatch = youTube->findBestMatch(_track, _onProgress);
        if (bestMatch.empty()) {
            _onProgress(-1);
            return;
        }
        _onProgress(10);

        auto trackName = trackPath + _track.get_id() + ".mp3";

        // download resource
        std::string command =
            "yt-dlp -f bestaudio --extract-audio --audio-format mp3 "
            "--audio-quality 0 -o '" +
            trackName + "' https://www.youtube.com/watch?v=" + bestMatch +
            " 2>&1";

        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe) {
            _onProgress(-1);
            return;
        }

        // Read output one character at a time to catch lines ending in '\n' or
        // '\r'
        std::string lineBuffer;
        char ch;
        int preparationpercent(10);
        while (fread(&ch, 1, 1, pipe) == 1) {
            if (ch == '\n' || ch == '\r') {
                if (!lineBuffer.empty()) {
                    // Process the full line (either full message or progress
                    // line)
                    // std::cout << lineBuffer << std::endl;
                    std::smatch match;
                    std::regex preparationRegex(R"(\[youtube\]\s+(\w*):.*)");
                    std::regex percentRegex(R"(\[download\]\s+(\d+\.\d+)%.*)");
                    if (std::regex_search(lineBuffer, match,
                                          preparationRegex)) {
                        preparationpercent++;
                        _onProgress(preparationpercent); // Progress percentage
                    } else if (std::regex_search(lineBuffer, match,
                                                 percentRegex)) {
                        int percent =
                            preparationpercent +
                            (std::stof(match[1].str()) / 100.0f) * 80.0f;
                        _onProgress(percent); // Progress percentage
                    }

                    lineBuffer.clear();
                }
            } else {
                lineBuffer += ch;
            }
        }

        int status = pclose(pipe);
        if (status != 0) {
            _onProgress(-1);
            return;
        }

        spotify->loadAdditionalData(_track);

        if (!_track.writeID3V2Tags(trackName.c_str())) {
            _onProgress(-1);
            return;
        }
        _onProgress(95);

        if (!_track.setAlbumCover(
                trackName,
                spotify->downloadImage(_track.get_album().get_imageUrl()))) {
            _onProgress(-1);
            return;
        }
        _onProgress(99);
        makeBlocked(_track);
        _onProgress(100);
    }).detach();
}

void Downloader::deleteLocalTrack(const std::filesystem::path &_filepath) {
    std::string possibleId(_filepath.stem().string());

    if (isBlocked(possibleId)) {
        blacklist["blacklist"].erase(possibleId);
    } else {
        std::cout << possibleId << " is not included in blacklist!"
                  << std::endl;
    }
    if (std::filesystem::exists(_filepath)) {
        if (std::filesystem::remove(_filepath)) {
            std::cout << "File deleted successfully.\n";
        } else {
            std::cerr << "Could not remove " << _filepath
                      << ":Failed to delete the file.\n";
        }
    } else {
        std::cerr << "Could not remove " << _filepath
                  << ": File does not exist.\n";
    }
}
