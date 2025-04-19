#if !defined(SPOTIFY_API_H)
#define SPOTIFY_API_H

#include <nlohmann/json.hpp>
#include <sstream>
#include <exception>
#include <string>

#include "Album.h"
#include "Artist.h"
#include "Playlist.h"
#include "Query.h"
#include "Track.h"
#include "User.h"

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
    enum searchItem_type {
        ALBUM,
        ARTIST,
        PLAYLIST,
        TRACK
        // SHOW,
        // EPISODE,
        // AUDIOBOOK
    };

    std::vector<Track> searchTrack(const std::string &_query,
                                   const std::string &_market = "",
                                   const std::string &_limit = "",
                                   const std::string &_offset = "");
    std::vector<Album> searchAlbum(const std::string &_query,
                                   const std::string &_market = "",
                                   const std::string &_limit = "",
                                   const std::string &_offset = "");
    std::vector<Artist> searchArtist(const std::string &_query,
                                     const std::string &_market = "",
                                     const std::string &_limit = "",
                                     const std::string &_offset = "");
    std::vector<Playlist> searchPlaylist(const std::string &_query,
                                         const std::string &_market = "",
                                         const std::string &_limit = "",
                                         const std::string &_offset = "");

    Album getAlbum(const std::string &_id);
    Artist getArtist(const std::string &_id);
    Playlist getPlaylist(const std::string &_id);
    Track getTrack(const std::string &_id);
    std::vector<Track> getAlbumTracks(const std::string &_id);
    std::vector<Track> getPlaylistTracks(const std::string &_id);
    std::string searchId(const std::string &_filename,
                         const std::string &_type);

  private:
    inline const static std::string urlAPI = "https://api.spotify.com/v1/";
    inline const static std::string urlToken =
        "https://accounts.spotify.com/api/token";

    json handleRequest(const std::string &_request);

    json search(searchItem_type _type, const std::string &_query,
                const std::string &_market = "", const std::string &_limit = "",
                const std::string &_offset = "");

    Album createAlbum(const json &_jsonAlbum, bool _fullTags = true);
    Artist createArtist(const json &_jsonArtist) const;
    std::vector<Artist> createArtists(const json &_jsonArtists) const;
    Playlist createPlaylist(const json &_jsonPlaylist) const;
    Track createTrack(const json &_jsonTrack, const Album &_album) const;
    User createUser(const json &_jsonUser) const;
};

} // namespace Spotify

#endif // SPOTIFY_API_H
