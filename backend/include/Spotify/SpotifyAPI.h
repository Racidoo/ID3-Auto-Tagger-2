#if !defined(SPOTIFY_API_H)
#define SPOTIFY_API_H

// #include "../lib/base64/base64.h"
// #include <chrono>
// #include <curl/curl.h>
// #include <filesystem>
// #include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

#include "Album.h"
#include "Artist.h"
#include "Query.h"
#include "Track.h"

using json = nlohmann::json;

namespace Spotify {

class SpotifyAPI : public Query {
  public:
    SpotifyAPI();
    ~SpotifyAPI();

    static SpotifyAPI &getInstance() {
        static SpotifyAPI instance;
        return instance;
    }

    json searchTrack(const std::string &_query);

    Track getTrack(const std::string &_id);
    // Album getAlbum(const std::string &_id);
    std::vector<Track> getAlbumTracks(const std::string &_id);
    std::vector<Track> getPlaylistTracks(const std::string &_id);
    std::string searchId(const std::string &_filename,
                         const std::string &_type);

    // inline static std::filesystem::path getExecutableDir() {
    //     std::filesystem::path exePath =
    //         std::filesystem::canonical("/proc/self/exe"); // Linux-specific
    //     return exePath.parent_path(); // Get directory containing the
    //     executable
    // }
    // inline const static std::filesystem::path pathCredentials =
    //     std::filesystem::current_path() / ".." / "api" / "credentials.json";

  private:
    // Private constructor and copy constructor to ensure only one instance
    // SpotifyAPI(const SpotifyAPI&) = delete;
    // SpotifyAPI& operator=(const SpotifyAPI&) = delete;

    enum searchItem_type {
        ALBUM,
        ARTIST,
        PLAYLIST,
        TRACK,
        SHOW,
        EPISODE,
        AUDIOBOOK
    };
    inline const static std::string urlAPI = "https://api.spotify.com/v1/";
    inline const static std::string urlToken =
        "https://accounts.spotify.com/api/token";

    // CURL *curl;
    // struct curl_slist *headers;

    // std::string clientId;
    // std::string clientSecret;
    // std::string accessToken;
    // std::chrono::steady_clock::time_point tokenExpirationTime;

    // std::string defaultMarket;
    // std::string defaultLimit;
    // std::string defaultOffset;

    // bool isTokenValid() const;
    // std::string getValidToken();
    bool authenticate();
    // const std::string getSpotifyAccessToken();
    json handleRequest(const std::string &_request);
    bool executeRequest(std::string &response_data);

    json search(searchItem_type _type, const std::string &_q,
                const std::string &_market = "", const std::string &_limit = "",
                const std::string &_offset = "");

    Album createAlbum(const json &_jsonAlbum, bool _fullTags = true);
    std::vector<Artist> createArtists(const json &_jsonArtists) const;
    Track createTrack(const json &_jsonTrack, const Album &_album) const;
};

} // namespace Spotify

#endif // SPOTIFY_API_H
