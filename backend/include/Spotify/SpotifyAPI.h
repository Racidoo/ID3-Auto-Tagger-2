#if !defined(SPOTIFY_API_H)
#define SPOTIFY_API_H

#include <exception>
#include <memory>
#include <nlohmann/json.hpp>
#include <regex>
#include <sstream>
#include <string>

// #include "Interfaces/IAlbum.h"
// #include "Interfaces/ITrack.h"
#include "IMediaService.hpp"
#include "Query.h"
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

    enum searchItem_type {
        ALBUM,
        ARTIST,
        PLAYLIST,
        TRACK
        // SHOW,
        // EPISODE,
        // AUDIOBOOK
    };

    struct TrackSearchContext {
        std::string title;
        std::string artist;
        std::string album;
        std::string filename;
        std::size_t durationSeconds = 0;
    };

    std::vector<Track> searchTrack(const std::string &_query,
                                   const std::string &_market = "",
                                   unsigned int _limit = 0,
                                   const std::string &_offset = "");
    std::vector<Album> searchAlbum(const std::string &_query,
                                   const std::string &_market = "",
                                   unsigned int _limit = 0,
                                   const std::string &_offset = "");
    std::vector<Artist> searchArtist(const std::string &_query,
                                     const std::string &_market = "",
                                     unsigned int _limit = 0,
                                     const std::string &_offset = "");
    std::vector<Playlist> searchPlaylist(const std::string &_query,
                                         const std::string &_market = "",
                                         unsigned int _limit = 0,
                                         const std::string &_offset = "");

    Album getAlbum(const std::string &_id);
    Artist getArtist(const std::string &_id);
    Playlist getPlaylist(const std::string &_id);
    Track getTrack(const std::string &_id);
    std::vector<Track> getAlbumTracks(const std::string &_id);
    std::vector<Track> getPlaylistTracks(const std::string &_id);
    std::string searchId(const TrackSearchContext &_ctx);

    void load(IMediaEntity &_obj) override;

    void loadAdditionalData(Artist &_artist);
    void loadAdditionalData(Album &_album);
    void loadAdditionalData(Playlist &_playlist);
    void loadAdditionalData(Track &_track);

    std::shared_ptr<Track> researchTags(const TrackSearchContext &_ctx);

    static bool isValidIdFormat(const std::string &_id);

  private:
    inline const static std::string urlAPI = "https://api.spotify.com/v1";
    std::string accessToken;
    std::string clientId;
    std::string clientSecret;

    void prepareHeaders(struct curl_slist *&_headers) override;
    std::string prepareUrl(const std::string &_url) override;

    json search(searchItem_type _type, const std::string &_query,
                const std::string &_market = "", unsigned int _limit = 0,
                const std::string &_offset = "");

    bool insertTracklist(Album &_album, const json &_jsonAlbum);
    bool insertLabel(Album &_album, const json &_jsonAlbum);
    bool insertCopyright(Album &_album, const json &_jsonAlbum);

    Album createAlbum(const json &_jsonAlbum, bool _fullTags = false);
    Artist createArtist(const json &_jsonArtist) const;
    std::vector<Artist> createArtists(const json &_jsonArtists) const;
    Playlist createPlaylist(const json &_jsonPlaylist) const;
    Track createTrack(const json &_jsonTrack, const Album &_album) const;
    User createUser(const json &_jsonUser) const;
};

} // namespace Spotify

#endif // SPOTIFY_API_H
