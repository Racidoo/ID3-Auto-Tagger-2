#include "../include/Downloader.h"
#include "../include/LocalTrack.h"

Downloader::Downloader()
    : trackPath(std::filesystem::current_path() / "music/"), spotify(nullptr),
      youTube(nullptr)
//   ,coverPath(std::filesystem::current_path() / "cover/")
{
    initialize();
    loadOrCreateBlacklist();
    loadOrCreateConfig();
}

Downloader::~Downloader() {
    writeBlacklist();
    writeConfig();
    if (spotify)
        delete spotify;
    if (youTube)
        delete youTube;
}

const std::filesystem::path &Downloader::get_trackPath() const {
    return trackPath;
}

void Downloader::set_trackPath(const std::filesystem::path &_path) {
    trackPath = _path;
}

bool Downloader::is_initialized() const { return (spotify && youTube); }

bool Downloader::initialize() {

    return initializeSpotify() && initializeYouTube();
    //  && initializeSoundcloud();
}

bool Downloader::initializeSpotify(const std::string &_clientId,
                                   const std::string &_clientSecret) {
    if (!spotify) {
        std::cout << "Initializing Spotify...";
        Spotify::SpotifyAPI *temp =
            (_clientId.empty() && _clientSecret.empty()
                 ? new Spotify::SpotifyAPI()
                 : new Spotify::SpotifyAPI(_clientId, _clientSecret));
        if (temp->is_initialized()) {
            std::cout << " success!" << std::endl;
            spotify = temp;
        } else {
            std::cout << " failed!" << std::endl;
            delete temp;
        }
    }
    return spotify;
}

bool Downloader::initializeYouTube(const std::string &_googleAuthToken) {
    if (!youTube) {
        std::cout << "Initializing YouTube...";
        YouTube::YouTubeAPI *temp =
            (_googleAuthToken.empty()
                 ? new YouTube::YouTubeAPI()
                 : new YouTube::YouTubeAPI(_googleAuthToken));
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
             json::object()} // "blacklist" key maps to an empty array
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

bool Downloader::loadOrCreateConfig() {
    const std::filesystem::path path = "config.json";

    if (!std::filesystem::exists(path)) {
        json config = {
            {"trackPath", std::filesystem::current_path() / "music/"}};
        std::ofstream file(path);
        file << config.dump(4);
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open config.json!" << std::endl;
        return false;
    }

    json config = json::parse(file);

    if (config.contains("trackPath") && config.at("trackPath").is_string()) {
        trackPath = config.at("trackPath").get<std::string>();
    } else {
        std::cerr << "Could not set custom trackPath" << std::endl;
    }

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

bool Downloader::writeConfig() const {
    std::ofstream configFile("config.json");
    if (!configFile.is_open()) {
        std::cerr << "Could not write config.json!" << std::endl;
        return false;
    }
    json config = {{"trackPath", trackPath.string()}};
    configFile << config.dump(4);
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
                          const std::string &_market, unsigned int _limit,
                          const std::string &_offset) {

    std::regex spotifyUrlPattern(
        R"(https:\/\/open\.spotify\.com\/(?:intl-[a-z]{2}\/)?(playlist|album|track)\/([\w]+))");
    std::regex youTubeUrlPattern(
        R"(youtu(?:\.be\/|be\.com\/watch\?v=)([A-Za-z0-9_-]{11}))");

    // std::string urlType;
    SearchResult result;
    std::smatch matches;
    std::string type, id;
    if (std::regex_search(_query, matches, spotifyUrlPattern)) {
        if (matches[1].matched && matches[2].matched) {
            // urlType = "Spotify";
            type = matches[1].str();
            id = matches[2].str();
        }
    } else if (std::regex_search(_query, matches, youTubeUrlPattern)) {
        if (matches[1].matched) {
            // urlType = "YouTube";
            id = matches[1].str();
            type = "video";
        }
    }

    if (type == "track") {
        auto track = spotify->getTrack(id);
        result.tracks.push_back(std::make_shared<Spotify::Track>(track));
        track.set_downloaded(isBlocked(track.get_id()));
    } else if (type == "album") {
        auto tracks = spotify->getAlbumTracks(id);
        for (auto &&track : tracks) {
            track.set_downloaded(isBlocked(track.get_id()));
            result.tracks.push_back(std::make_shared<Spotify::Track>(track));
        }
    } else if (type == "playlist") {
        auto tracks = spotify->getPlaylistTracks(id);
        for (auto &&track : tracks) {
            track.set_downloaded(isBlocked(track.get_id()));
            result.tracks.push_back(std::make_shared<Spotify::Track>(track));
        }
    } else if (type == "video") {
        auto video = youTube->getVideo(id);
        result.videos.push_back(std::move(video));

    } else {

        for (auto &&cat : categories) {
            switch (cat) {
            case SearchCategory::Track: {
                auto tracks =
                    spotify->searchTrack(_query, _market, _limit, _offset);
                for (auto &track : tracks) {
                    track.set_downloaded(isBlocked(track.get_id()));
                    result.tracks.push_back(
                        std::make_shared<Spotify::Track>(track));
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
            case SearchCategory::Video: {
                auto videos = youTube->searchVideo(_query, nullptr, _limit);
                for (auto &&v : videos) {
                    result.videos.push_back(std::move(v));
                }
                break;
            }
            }
        }
    }
    return std::move(result);
}

std::string Downloader::downloadResource(std::vector<YouTube::Video> &&_videos,
                                         std::function<void(int)> _onProgress) {
    std::string result;
    for (auto &&video : _videos) {
        downloadAndTag(video, _onProgress);
    }
    return result;
}

void Downloader::downloadResource(
    const std::vector<std::shared_ptr<TrackInterface>> &_tracks,
    std::function<void(int)> _onProgress) {

    for (auto &&track : _tracks) {
        downloadAndTag(track, _onProgress);
    }
}

bool Downloader::isBlocked(const std::string &_id) const {
    return blacklist.at("blacklist").contains(_id);
}

void Downloader::makeBlocked(const Spotify::Track &_track) {
    blacklist["blacklist"][_track.get_id()]["title"] = _track.get_name();
    blacklist["blacklist"][_track.get_id()]["artist"] =
        _track.get_stringArtists();
}

void Downloader::makeBlocked(std::shared_ptr<TrackInterface> _data) {
    if (!_data || !_data->get_localTrack())
        return;
    const auto &filename = _data->get_id();
    if (!Spotify::SpotifyAPI::isValidIdFormat(filename)) {
        return;
    }

    blacklist["blacklist"][filename]["title"] = _data->get_title();
    blacklist["blacklist"][filename]["artist"] = _data->get_artist();
}

void Downloader::removeBlocked(std::shared_ptr<TrackInterface> _data) {

    if (!_data || !_data->get_localTrack())
        return;
    const auto &filename = _data->get_id();

    if (isBlocked(filename)) {
        blacklist["blacklist"].erase(filename);
        std::cout << "removed '" << filename << "' from blocklist" << std::endl;
    }
}

void Downloader::downloadAndTag(std::shared_ptr<TrackInterface> _track,
                                std::function<void(int)> _onProgress) {

    if (!_track || !_track->get_spotifyTrack()) {
        std::cerr << "Cannot download with missing spotify information!"
                  << std::endl;
        _onProgress(-1);
        return;
    }

    std::thread([this, _onProgress, _track]() mutable {
        _onProgress(0);
        auto bestMatch =
            youTube->findBestMatch(*_track->get_spotifyTrack(), _onProgress);
        if (bestMatch.empty()) {
            _onProgress(-1);
            return;
        }
        _onProgress(10);

        std::filesystem::path trackName =
            trackPath / (_track->get_id() + ".mp3");

        // download resource
        std::string command =
            "yt-dlp -f bestaudio --extract-audio --audio-format "
            "mp3 "
            "--audio-quality 0 -o '" +
            trackName.string() +
            "' https://www.youtube.com/watch?v=" + bestMatch + " 2>&1";

        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe) {
            _onProgress(-1);
            return;
        }

        // Read output one character at a time to catch lines ending
        // in '\n' or
        // '\r'
        std::string lineBuffer;
        char ch;
        int preparationpercent(10);
        while (fread(&ch, 1, 1, pipe) == 1) {
            if (ch == '\n' || ch == '\r') {
                if (!lineBuffer.empty()) {
                    // Process the full line (either full message or
                    // progress line) std::cout << lineBuffer <<
                    // std::endl;
                    std::smatch match;
                    std::regex preparationRegex(R"(\[youtube\]\s+(\w*):.*)");
                    std::regex percentRegex(R"(\[download\]\s+(\d+\.\d+)%.*)");
                    if (std::regex_search(lineBuffer, match,
                                          preparationRegex)) {
                        preparationpercent++;
                        _onProgress(preparationpercent); // Progress
                                                         // percentage
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

        auto localTrack(
            TrackInterface::fromLocal(std::make_shared<LocalTrack>(trackName)));
        // get additianl tags that are not relevant in normal search
        spotify->loadAdditionalData(_track);
        localTrack->verifyTags(_track);

        _onProgress(99);
        makeBlocked(localTrack);
        _onProgress(100);
    }).detach();
}

void Downloader::downloadAndTag(YouTube::Video &_video,
                                std::function<void(int)> _onProgress) {

    std::thread([this, _onProgress, _video]() mutable {
        _onProgress(0);
        // auto bestMatch = youTube->findBestMatch(_track, _onProgress);
        // if (bestMatch.empty()) {
        //     _onProgress(-1);
        //     return;
        // }
        _onProgress(10);

        std::filesystem::path trackName =
            trackPath / (_video.get_id() + ".mp3");

        // download resource
        std::string command =
            "yt-dlp -f bestaudio --extract-audio --audio-format mp3 "
            "--audio-quality 0 -o '" +
            trackName.string() +
            "' https://www.youtube.com/watch?v=" + _video.get_id() + " 2>&1";

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
            _onProgress(-2);
            return;
        }

        // spotify->loadAdditionalData(_track);

        // if (!_track.writeID3V2Tags(trackName.c_str())) {
        //     _onProgress(-1);
        //     return;
        // }
        _onProgress(95);

        // if (!_track.setAlbumCover(
        //         trackName,
        //         spotify->downloadImage(_track.get_album().get_imageUrl()))) {
        //     _onProgress(-1);
        //     return;
        // }
        _onProgress(99);
        // makeBlocked(_track);
        _onProgress(100);
    }).detach();
}

void Downloader::deleteLocalTrack(std::shared_ptr<TrackInterface> _data) {
    if (!_data || !_data->get_localTrack()) {
        std::cerr << "invalid reference to localTrack!" << std::endl;
        return;
    }

    const auto &filepath = _data->get_localTrack()->get_filepath();
    std::string possibleId(filepath.stem().string());

    if (!_data->get_localTrack()->deleteLocalTrack()) {
        return;
    }

    removeBlocked(_data);
}