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
    Spotify::SpotifyAPI spotify;
    YouTube youTube;

    std::string trackPath;
    // std::string coverPath;

    json blacklist;

    bool readBlacklist();
    bool writeBlacklist() const;

    bool isBlocked(const std::string &_id) const;
    void makeBlocked(const Spotify::Track &_track);

    void downloadAndTag(const Spotify::Track &_track,
                        std::function<void(int)> _onProgress);

  public:
    Downloader(/* args */);
    ~Downloader();

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
                               const std::set<SearchCategory> &categories = {});
    std::string downloadResource(const std::vector<Spotify::Track> &_tracks,
                                 std::function<void(int)> _onProgress);
};

#endif // DOWNLOADER_H
