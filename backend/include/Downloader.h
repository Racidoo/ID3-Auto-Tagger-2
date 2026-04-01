#if !defined(DOWNLOADER_H)
#define DOWNLOADER_H

#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <vector>
// #include <thread>
#include <future>
#include <taglib/mpegfile.h>

#include "TrackInterface.h"
#include "Spotify/SpotifyAPI.h"
#include "YouTube.h"

class Downloader {
  private:
    Spotify::SpotifyAPI *spotify;
    YouTube *youTube;

    std::filesystem::path trackPath;
    // std::string coverPath;

    json blacklist;

    bool loadOrCreateBlacklist();
    bool loadOrCreateConfig();
    bool writeBlacklist() const;
    bool writeConfig() const;

    void makeBlocked(const Spotify::Track &_track);

    void downloadAndTag(std::shared_ptr<TrackInterface::TrackViewData> _track,
                        std::function<void(int)> _onProgress);

  public:
    Downloader(/* args */);
    ~Downloader();

    Spotify::SpotifyAPI *get_spotify() { return spotify; }
    YouTube *get_youtube() { return youTube; }

    const std::filesystem::path &get_trackPath() const;
    void set_trackPath(const std::filesystem::path &_path);

    bool is_initialized() const;
    bool initialize();
    bool initializeSpotify(const std::string &_spotifyclientId = "",
                           const std::string &_spotifyClientSecret = "");
    bool initializeYouTube(const std::string &_googleAuthToken = "");

    enum class SearchCategory {
        Track,
        Album,
        Artist,
        Playlist,
        // Add more categories here if needed
    };

    struct SearchResult {
        std::vector<Spotify::Track> tracks;
        std::vector<Spotify::Album> albums;
        std::vector<Spotify::Artist> artists;
        std::vector<Spotify::Playlist> playlists;
    };

    bool isBlocked(const std::string &_id) const;
    void makeBlocked(std::shared_ptr<TrackInterface::TrackViewData> _data);

    // void verifyTags();
    SearchResult fetchResource(const std::string &_query,
                               const std::set<SearchCategory> &categories = {},
                               const std::string &_market = "DE",
                               const std::string &_limit = "5",
                               const std::string &_offset = "0");
    void downloadResource(
        const std::vector<std::shared_ptr<TrackInterface::TrackViewData>>
            &_tracks,
        std::function<void(int)> _onProgress);
    void deleteLocalTrack(const std::filesystem::path &_filepath);
    // void verifyLocalResource(std::filesystem::path *_filepath);
};

#endif // DOWNLOADER_H
