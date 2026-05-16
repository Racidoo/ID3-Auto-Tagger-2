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

#include "Discogs/DiscogsAPI.h"
#include "Interfaces/ISearchResult.hpp"
#include "Spotify/SpotifyAPI.h"
#include "YouTube/YouTubeAPI.h"

class IAlbum;
class IArtist;
class ITrack;
class IPlaylist;
class IVideo;

class Downloader {
  private:
    Spotify::SpotifyAPI *spotify;
    YouTube::YouTubeAPI *youTube;
    Discogs::DiscogsAPI *discogs;

    std::filesystem::path trackPath;
    // std::string coverPath;

    json blacklist;

    bool loadOrCreateBlacklist();
    bool loadOrCreateConfig();
    bool writeBlacklist() const;
    bool writeConfig() const;

    void makeBlocked(const Spotify::Track &_track);

    void downloadAndTag(std::shared_ptr<ITrack> _track,
                        std::function<void(int)> _onProgress);

    void downloadAndTag(std::shared_ptr<IVideo> &_video,
                        std::function<void(int)> _onProgress);

  public:
    Downloader(/* args */);
    ~Downloader();

    inline Spotify::SpotifyAPI *get_spotify() const { return spotify; }
    inline YouTube::YouTubeAPI *get_youtube() const { return youTube; }
    inline Discogs::DiscogsAPI *get_discogs() const { return discogs; }

    const std::filesystem::path &get_trackPath() const;
    void set_trackPath(const std::filesystem::path &_path);

    bool is_initialized() const;
    bool initialize();
    bool initializeSpotify(const std::string &_clientId = "",
                           const std::string &_clientSecret = "");
    bool initializeYouTube(const std::string &_accessToken = "");
    bool initializeDiscogs(const std::string &_accessToken = "");

    bool isBlocked(const std::string &_id) const;
    void makeBlocked(std::shared_ptr<ITrack> _data);
    void removeBlocked(std::shared_ptr<ITrack> _data);

    // void verifyTags();
    ISearchResult fetchResource(
        const std::string &_query,
        const std::set<ISearchResult::SearchCategory> &categories = {},
        const std::string &_market = "DE", unsigned int _limit = 0,
        const std::string &_offset = "0");
    void downloadResource(std::shared_ptr<ITrack> _track,
                          std::function<void(int)> _onProgress);
    std::string downloadResource(std::vector<std::shared_ptr<IVideo>> &&_videos,
                                 std::function<void(int)> _onProgress);
    // void deleteLocalTrack(std::shared_ptr<ITrack> _data);
};

#endif // DOWNLOADER_H
