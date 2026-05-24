#if !defined(SPOTIFY_API_H)
#define SPOTIFY_API_H

#include <exception>
#include <memory>
#include <nlohmann/json.hpp>
#include <regex>
#include <sstream>
#include <string>

#include "Query.h"
#include "Services/IMediaService.hpp"
#include "Spotify/Album.h"
#include "Spotify/Artist.h"
#include "Spotify/Playlist.h"
#include "Spotify/Track.h"
#include "Spotify/User.h"

using json = nlohmann::json;

namespace Spotify {

class SpotifyAPI : public Query, public IMediaService {
  public:
    SpotifyAPI();
    SpotifyAPI(const std::string &_clientId, const std::string &_clientSecret);
    ~SpotifyAPI() = default;

    void saveCredentials() override;
    std::string generateAccessToken() override;
    inline constexpr IMediaService::MediaSourceId get_id() const override {
        return MediaSourceId::Spotify;
    }
    enum searchItem_type {
        ALBUM,
        ARTIST,
        PLAYLIST,
        TRACK
        // SHOW,
        // EPISODE,
        // AUDIOBOOK
    };

    std::vector<std::shared_ptr<ITrack>>
    searchTrack(const std::string &_query, const std::string &_market = "",
                unsigned int _limit = 0, const std::string &_offset = "");
    std::vector<std::shared_ptr<IAlbum>>
    searchAlbum(const std::string &_query, const std::string &_market = "",
                unsigned int _limit = 0, const std::string &_offset = "");
    std::vector<std::shared_ptr<IArtist>>
    searchArtist(const std::string &_query, const std::string &_market = "",
                 unsigned int _limit = 0, const std::string &_offset = "");
    std::vector<std::shared_ptr<IPlaylist>>
    searchPlaylist(const std::string &_query, const std::string &_market = "",
                   unsigned int _limit = 0, const std::string &_offset = "");

    std::shared_ptr<IAlbum> getAlbum(const std::string &_id);
    std::shared_ptr<IArtist> getArtist(const std::string &_id);
    std::shared_ptr<IPlaylist> getPlaylist(const std::string &_id);
    std::shared_ptr<ITrack> getTrack(const std::string &_id);
    std::vector<std::shared_ptr<ITrack>> getAlbumTracks(const std::string &_id);
    std::vector<std::shared_ptr<ITrack>>
    getPlaylistTracks(const std::string &_id);
    // std::string searchId(const TrackSearchContext &_ctx);

    bool load(std::shared_ptr<IMediaEntity> _obj) override;

    bool loadAdditionalData(std::shared_ptr<Artist> _artist);
    bool loadAdditionalData(std::shared_ptr<Album> _album);
    bool loadAdditionalData(std::shared_ptr<Playlist> _playlist);
    bool loadAdditionalData(std::shared_ptr<Track> _track);

    // std::shared_ptr<ITrack> researchTags(const TrackSearchContext &_ctx);

    static bool isValidIdFormat(const std::string &_id);

  private:
    inline const static std::string urlAPI = "https://api.spotify.com/v1";
    std::string accessToken;
    std::string clientId;
    std::string clientSecret;

    std::unordered_map<std::string, std::shared_ptr<Album>> cachedAlbums;
    std::unordered_map<std::string, std::shared_ptr<Artist>> cachedArtists;
    std::unordered_map<std::string, std::shared_ptr<Track>> cachedTracks;

    void prepareHeaders(struct curl_slist *&_headers) override;
    std::string prepareUrl(const std::string &_url) override;

    json search(searchItem_type _type, const std::string &_query,
                const std::string &_market = "", unsigned int _limit = 0,
                const std::string &_offset = "");

    bool insertTracklist(std::shared_ptr<Album> _album, const json &_jsonAlbum);
    bool insertLabel(std::shared_ptr<Album> _album, const json &_jsonAlbum);
    bool insertCopyright(std::shared_ptr<Album> _album, const json &_jsonAlbum);

    IAlbum::album_type_t parseAlbumType(const json &_jsonAlbum);

    std::shared_ptr<Album> createAlbum(const json &_jsonAlbum,
                                       bool _fullTags = false);
    std::shared_ptr<IArtist> createArtist(const json &_jsonArtist);
    std::vector<std::shared_ptr<IArtist>>
    createArtists(const json &_jsonArtists);
    std::shared_ptr<IPlaylist> createPlaylist(const json &_jsonPlaylist);
    std::shared_ptr<ITrack> createTrack(const json &_jsonTrack,
                                        std::shared_ptr<Album> _album);
    std::shared_ptr<User> createUser(const json &_jsonUser);
};

} // namespace Spotify

#endif // SPOTIFY_API_H
