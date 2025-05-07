#if !defined(DOWNLOADER_H)
#define DOWNLOADER_H

#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <vector>
// #include <thread>
#include <future>

#include "Spotify/SpotifyAPI.h"
#include "YouTube.h"

class Downloader {
  private:
    Spotify::SpotifyAPI *spotify;
    YouTube *youTube;

    std::string trackPath;
    // std::string coverPath;

    json blacklist;

    // bool readBlacklist();
    bool loadOrCreateBlacklist();
    bool writeBlacklist() const;

    bool isBlocked(const std::string &_id) const;
    void makeBlocked(const Spotify::Track &_track);

    void downloadAndTag(Spotify::Track &_track,
                        std::function<void(int)> _onProgress);

  public:
    Downloader(/* args */);
    ~Downloader();

    Spotify::SpotifyAPI *get_spotify() { return spotify; }
    YouTube *get_youtube() { return youTube; }

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

    // void verifyTags();
    SearchResult fetchResource(const std::string &_query,
                               const std::set<SearchCategory> &categories = {},
                               const std::string &_market = "DE",
                               const std::string &_limit = "5",
                               const std::string &_offset = "0");
    std::string downloadResource(std::vector<Spotify::Track> &&_tracks,
                                 std::function<void(int)> _onProgress);
    void deleteLocalTrack(const std::filesystem::path &_filepath);
};

#endif // DOWNLOADER_H
