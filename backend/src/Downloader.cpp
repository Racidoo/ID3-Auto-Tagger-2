#include "../include/Downloader.h"

Downloader::Downloader()
    : trackPath(std::filesystem::current_path() / "music/")
//   ,coverPath(std::filesystem::current_path() / "cover/")
{
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
        _tracks.front().set_downloaded(isBlocked(_tracks.front().get_id()));
    } else if (type == "album") {
        for (auto &&track : spotify.getAlbumTracks(id)) {
            track.set_downloaded(isBlocked(track.get_id()));
            _tracks.push_back(track);
        }
    } else if (type == "playlist") {
        for (auto &&track : spotify.getPlaylistTracks(id)) {
            track.set_downloaded(isBlocked(track.get_id()));
            _tracks.push_back(track);
        }
    } else if (urlType == "YouTube") {
    }
    return urlType;
}

std::string
Downloader::downloadResource(const std::vector<Spotify::Track> &_tracks,
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

void Downloader::downloadAndTag(const Spotify::Track &_track,
                                std::function<void(int)> _onProgress) {

    std::thread([this, _onProgress, _track]() {
        _onProgress(0);
        auto bestMatch = youTube.findBestMatch(_track);
        if (bestMatch.empty()) {
            _onProgress(-1);
            return;
        }
        _onProgress(10);

        auto trackName = trackPath + _track.get_id() + ".mp3";
        // auto coverName = coverPath + _track.get_id() + ".jpg";

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
        while (fread(&ch, 1, 1, pipe) == 1) {
            if (ch == '\n' || ch == '\r') {
                if (!lineBuffer.empty()) {
                    // Process the full line (either full message or progress
                    // line)
                    std::smatch match;
                    std::regex percentRegex(R"(\[download\]\s+(\d+\.\d+)%.*)");
                    if (std::regex_search(lineBuffer, match, percentRegex)) {
                        int percent =
                            10.0f +
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
        _onProgress(90);
        if (!_track.writeID3V2Tags(trackName.c_str()) ||
            !_track.setAlbumCover(
                trackName,
                spotify.downloadImage(_track.get_album().get_imageUrl()))) {
            return;
        }
        _onProgress(99);
        makeBlocked(_track);
        _onProgress(100);
    }).detach();
}