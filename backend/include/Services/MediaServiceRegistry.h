#pragma once

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Local/LocalTrackService.h"
#include "Services/SearchService.h"
#include "Services/Sources/DiscogsMediaSource.h"
#include "Services/Sources/SpotifyMediaSource.h"
#include "Services/Sources/YoutubeMediaSource.h"
#include "Services/TrackVerificationIndex.h"

class IMediaService;
namespace Spotify {
class SpotifyAPI;
} // namespace Spotify
namespace YouTube {
class YouTubeAPI;
} // namespace YouTube
namespace Discogs {
class DiscogsAPI;
} // namespace Discogs

class MediaServiceRegistry {
  public:
    MediaServiceRegistry();
    ~MediaServiceRegistry() = default;

    MediaServiceRegistry(const MediaServiceRegistry &) = delete;
    MediaServiceRegistry &operator=(const MediaServiceRegistry &) = delete;
    MediaServiceRegistry(MediaServiceRegistry &&) = delete;
    MediaServiceRegistry &operator=(MediaServiceRegistry &&) = delete;

    Spotify::SpotifyAPI *get_spotify();
    Discogs::DiscogsAPI *get_discogs();
    LocalTrackService *get_local();
    YouTube::YouTubeAPI *get_youtube();

    SearchService *get_searchService();
    TrackVerificationIndex *get_trackVerificationIndex();

    const std::filesystem::path &get_trackPath() const;
    void set_trackPath(const std::filesystem::path &_path);

    bool initialize();

    bool initializeSpotify(const std::string &_id = "",
                           const std::string &_secret = "");
    bool initializeYouTube(const std::string &_token = "");
    bool initializeDiscogs(const std::string &_token = "");
    bool initializeLocal();
    bool isInitialized() const;

    bool initializeSearchService();
    bool initializeTrackVerificationIndex();

    bool loadOrCreateConfig();
    bool writeConfig() const;

  private:
    std::unique_ptr<Spotify::SpotifyAPI> spotify;
    std::unique_ptr<Discogs::DiscogsAPI> discogs;
    std::unique_ptr<LocalTrackService> local;
    std::unique_ptr<YouTube::YouTubeAPI> youtube;

    std::unique_ptr<DiscogsMediaSource> discogsSource;
    std::unique_ptr<SpotifyMediaSource> spotifySource;
    std::unique_ptr<YoutubeMediaSource> youtubeSource;

    std::vector<IMediaSource *> sources;

    std::unique_ptr<SearchService> searchService;
    std::unique_ptr<TrackVerificationIndex> trackVerificationIndex;

    std::filesystem::path trackPath;

    template <typename T, typename... Args>
    bool initializeService(std::unique_ptr<T> &target, const std::string &name,
                           Args &&...args) {
        if (target)
            return true;

        std::cout << "Initializing " << name << "...";

        auto temp = std::make_unique<T>(std::forward<Args>(args)...);

        if (temp->generateAccessToken().empty()) {
            std::cout << " failed!\n";
            return false;
        }

        std::cout << " success!\n";
        target = std::move(temp);
        return true;
    }
};