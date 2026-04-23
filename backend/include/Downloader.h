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

#include "Spotify/SpotifyAPI.h"
#include "TrackInterface.h"
#include "YouTube/YouTubeAPI.h"

class Downloader {
  private:
    Spotify::SpotifyAPI *spotify;
    YouTube::YouTubeAPI *youTube;

    std::filesystem::path trackPath;
    // std::string coverPath;

    json blacklist;

    bool loadOrCreateBlacklist();
    bool loadOrCreateConfig();
    bool writeBlacklist() const;
    bool writeConfig() const;

    void makeBlocked(const Spotify::Track &_track);

    void downloadAndTag(std::shared_ptr<TrackInterface> _track,
                        std::function<void(int)> _onProgress);

    void downloadAndTag(YouTube::Video &_video,
                        std::function<void(int)> _onProgress);

  public:
    Downloader(/* args */);
    ~Downloader();

    inline Spotify::SpotifyAPI *get_spotify() const { return spotify; }
    inline YouTube::YouTubeAPI *get_youtube() const { return youTube; }
    inline YouTube::YouTubeAPI *get_beatport() const { return youTube; }

    const std::filesystem::path &get_trackPath() const;
    void set_trackPath(const std::filesystem::path &_path);

    bool is_initialized() const;
    bool initialize();
    bool initializeSpotify(const std::string &_clientId = "",
                           const std::string &_clientSecret = "");
    bool initializeYouTube(const std::string &_googleAuthToken = "");

    enum class SearchCategory {
        Track,
        Album,
        Artist,
        Playlist,
        Video
        // Add more categories here if needed
    };

    struct SearchResult {
        std::vector<std::shared_ptr<Spotify::Track>> tracks;
        std::vector<Spotify::Album> albums;
        std::vector<Spotify::Artist> artists;
        std::vector<Spotify::Playlist> playlists;
        std::vector<std::unique_ptr<YouTube::Video>> videos;
    };

    bool isBlocked(const std::string &_id) const;
    void makeBlocked(std::shared_ptr<TrackInterface> _data);
    void removeBlocked(std::shared_ptr<TrackInterface> _data);

    // void verifyTags();
    SearchResult fetchResource(const std::string &_query,
                               const std::set<SearchCategory> &categories = {},
                               const std::string &_market = "DE",
                               unsigned int _limit = 0,
                               const std::string &_offset = "0");
    void downloadResource(
        const std::vector<std::shared_ptr<TrackInterface>> &_tracks,
        std::function<void(int)> _onProgress);
    std::string downloadResource(std::vector<YouTube::Video> &&_videos,
                                 std::function<void(int)> _onProgress);
    void deleteLocalTrack(std::shared_ptr<TrackInterface> _data);
};

#endif // DOWNLOADER_H
