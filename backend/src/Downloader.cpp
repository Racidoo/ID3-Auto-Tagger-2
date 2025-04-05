#include "../include/Downloader.h"

Downloader::Downloader() {
    Spotify::SpotifyAPI spotify;
    YouTube youTube;
    readBlacklist();
}

Downloader::~Downloader() { writeBlacklist(); }

bool Downloader::readBlacklist() {
    std::ifstream blacklistFile("blacklist.json");
    if (!blacklistFile.is_open()) {
        std::cerr << "Could not read blacklist.json!" << std::endl;
        return false;
    }

    try {
        blacklist = json::parse(blacklistFile); // Parse only once
    } catch (const json::parse_error &e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
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

// std::string Downloader::determineOrigin(const std::string &_url) const{

// }

std::string Downloader::fetchResource(const std::string &_url,
                                      std::vector<Spotify::Track> &_tracks) {

    std::regex spotifyUrlPattern(
        R"(https:\/\/open\.spotify\.com\/(?:intl-[a-z]{2}\/)?(playlist|album|track)\/([\w]+))");
    std::regex youTubeUrlPattern(
        R"(https:\/\/www\.youtube\.com\/watch\?v=(\w*))");

    std::string urlType;

    std::smatch matches;
    std::string type, id;
    if (std::regex_search(_url, matches, spotifyUrlPattern)) {
        if (matches[1].matched && matches[2].matched) {
            urlType = "Spotify";
            type = matches[1].str();
            id = matches[2].str();
        }
    } else if (std::regex_search(_url, matches, youTubeUrlPattern)) {
        if (matches[1].matched && matches[2].matched) {
            urlType = "YouTube";
            id = matches[1].str();
        }
    }

    if (type == "track") {
        _tracks.push_back(spotify.getTrack(id));
    } else if (type == "album") {
        for (auto &&track : spotify.getAlbumTracks(id)) {
            _tracks.push_back(track);
        }
    } else if (type == "playlist") {
        for (auto &&track : spotify.getPlaylistTracks(id)) {
            _tracks.push_back(track);
        }
    } else if (urlType == "YouTube") {
    }
    return urlType;
}

std::string
Downloader::downloadResource(const std::vector<Spotify::Track> &_tracks) {
    std::string result;
    for (auto &&track : _tracks) {
        // verify with blacklist
        if (isBlocked(track.get_id())) {
            result += track.get_id() + ": Resource already downloaded!\n";
            continue;
        }
        if (!downloadAndTag(track)) {
            result +=
                track.get_id() + ": Error while downloading or tagging!\n";
            continue;
        }
        result += track.get_id() + ": Successfully downloaded and tagged " +
                  track.get_name() + " by " + track.get_stringArtists() + "\n";
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

bool Downloader::downloadAndTag(const Spotify::Track &_track) {
    auto bestMatch = youTube.findBestMatch(_track);

    std::string trackPath = (std::filesystem::current_path() / "music" /
                             (_track.get_id() + ".mp3"));
    std::string coverPath = (std::filesystem::current_path() / "cover" /
                             (_track.get_id() + ".png"));
    // download resource
    std::string command =
        "yt-dlp -f bestaudio --extract-audio --audio-format mp3 "
        "--audio-quality 0 -o '" +
        trackPath + "' https://www.youtube.com/watch?v=" + bestMatch;
    auto response = std::system(command.c_str());
    std::cout << command << "\t-> response: " << response << std::endl;
    if (response != 0)
        return false;

    if (!_track.writeID3V2Tags(trackPath.c_str()) ||
        !spotify.downloadImage(_track.get_album().get_imageUrl(), coverPath) ||
        !_track.setAlbumCover(trackPath, coverPath)) {
        return false;
    }
    makeBlocked(_track);
    return true;
}
