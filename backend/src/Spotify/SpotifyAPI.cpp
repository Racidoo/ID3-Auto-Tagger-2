#include "../include/Spotify/SpotifyAPI.h"

namespace Spotify {

SpotifyAPI::SpotifyAPI() : Query("Spotify") {}

SpotifyAPI::SpotifyAPI(const std::string &_clientId,
                       const std::string &_clientSecret)
    : Query("Spotify", _clientId, _clientSecret, "") {}

SpotifyAPI::~SpotifyAPI() {}

Album SpotifyAPI::createAlbum(const json &_jsonAlbum, bool _fullTags) {
    Album album(_jsonAlbum.at("id").get<std::string>(),
                _jsonAlbum.at("name").get<std::string>(),
                _jsonAlbum.at("album_type").get<std::string>(),
                _jsonAlbum.at("total_tracks").get<unsigned int>(),
                _jsonAlbum.at("release_date").get<std::string>(),
                _jsonAlbum.at("images").at(1).at("url").get<std::string>(),
                createArtists(_jsonAlbum.at("artists")));
    return album;
}

Artist SpotifyAPI::createArtist(const json &_jsonArtist) const {
    return Artist(_jsonArtist.at("id").get<std::string>(),
                  _jsonArtist.at("name").get<std::string>(),
                  _jsonArtist.contains("images") &&
                          !_jsonArtist.at("images").empty()
                      ? _jsonArtist.at("images")[1].at("url").get<std::string>()
                      : "");
}

std::vector<Artist> SpotifyAPI::createArtists(const json &_jsonArtists) const {
    std::vector<Artist> artists;
    for (auto &&artist : _jsonArtists) {
        artists.push_back(createArtist(artist));
    }
    return artists;
}

Playlist SpotifyAPI::createPlaylist(const json &_jsonPlaylist) const {
    return Playlist(_jsonPlaylist.at("id").get<std::string>(),
                    _jsonPlaylist.at("name").get<std::string>(),
                    _jsonPlaylist.at("images")[0].at("url").get<std::string>(),
                    createUser(_jsonPlaylist.at("owner")));
}

Track SpotifyAPI::createTrack(const json &_jsonTrack,
                              const Album &_album) const {
    return Track(_jsonTrack.at("id").get<std::string>(),
                 _jsonTrack.at("name").get<std::string>(),
                 _jsonTrack.at("disc_number").get<unsigned int>(),
                 _jsonTrack.at("duration_ms").get<unsigned long>(),
                 _jsonTrack.at("explicit").get<bool>(),
                 _jsonTrack.at("track_number").get<unsigned int>(), _album,
                 createArtists(_jsonTrack["artists"]));
}

User SpotifyAPI::createUser(const json &_jsonUser) const {
    return User(_jsonUser.at("id").get<std::string>(),
                _jsonUser.at("display_name").get<std::string>(),
                _jsonUser.contains("images")
                    ? _jsonUser.at("images")[1].at("url").get<std::string>()
                    : "");
}

Album SpotifyAPI::getAlbum(const std::string &_id) {
    return createAlbum(headerRequest(urlAPI + "albums/" + _id));
}

Artist SpotifyAPI::getArtist(const std::string &_id) {
    return createArtist(headerRequest(urlAPI + "artists/" + _id));
}

Playlist SpotifyAPI::getPlaylist(const std::string &_id) {
    return createPlaylist(headerRequest(urlAPI + "playlists/" + _id));
}

Track SpotifyAPI::getTrack(const std::string &_id) {
    json jsonTrack = headerRequest(urlAPI + "tracks/" + _id);
    json jsonArtist = jsonTrack.at("artists");
    json jsonAlbum = jsonTrack.at("album");

    return createTrack(jsonTrack, createAlbum(jsonAlbum));
}

std::vector<Track> SpotifyAPI::getAlbumTracks(const std::string &_id) {
    json jsonAlbum = headerRequest(urlAPI + "albums/" + _id);
    json jsonTracks = jsonAlbum.at("tracks");
    Album album = createAlbum(jsonAlbum);
    std::vector<Track> tracks;

    for (auto &&jsonTrack : jsonTracks["items"]) {
        tracks.push_back(createTrack(jsonTrack, album));
    }
    return tracks;
}

std::vector<Track> SpotifyAPI::getPlaylistTracks(const std::string &_id) {
    json jsonPlaylist = headerRequest(urlAPI + "playlists/" + _id);
    json jsonTracks = jsonPlaylist.at("tracks");
    std::vector<Track> tracks;

    for (auto &&jsonPlaylistEntry : jsonTracks["items"]) {
        auto jsonTrack = jsonPlaylistEntry.at("track");
        tracks.push_back(
            createTrack(jsonTrack, createAlbum(jsonTrack.at("album"))));
    }
    return tracks;
}

/**
 * @brief
 *
 * @param _type
 * @param _query
 * @param _market
 * @param _limit
 * @param _offset
 * @throws std::runtime_error - forwards HTML Error codes
 * @return json
 */
json SpotifyAPI::search(searchItem_type _type, const std::string &_query,
                        const std::string &_market, unsigned int _limit,
                        const std::string &_offset) {
    std::string typeStr;
    switch (_type) {
    case searchItem_type::TRACK:
        typeStr = "track";
        break;
    case searchItem_type::ALBUM:
        typeStr = "album";
        break;
    case searchItem_type::ARTIST:
        typeStr = "artist";
        break;
    case searchItem_type::PLAYLIST:
        typeStr = "playlist";
        break;
    }

    std::stringstream url;
    url << "https://api.spotify.com/v1/search?q="
        << curl_escape(_query.c_str(), 0) << "&type=" << typeStr;

    if (!_market.empty())
        url << "&market=" << _market;
    if (!_limit == 0)
        url << "&limit=" << _limit;
    if (!_offset.empty())
        url << "&offset=" << _offset;

    json result = headerRequest(url.str());
    if (result.contains("error")) {
        throw std::runtime_error(result.dump());
    }
    return result.at(typeStr + 's').at("items"); // e.g., result["tracks"]
}

std::vector<Track> SpotifyAPI::searchTrack(const std::string &_query,
                                           const std::string &_market,
                                           unsigned int _limit,
                                           const std::string &_offset) {
    std::vector<Track> tracks;
    for (auto &&jsonTrack : search(TRACK, _query, _market, _limit, _offset)) {
        tracks.push_back(
            createTrack(jsonTrack, createAlbum(jsonTrack.at("album"))));
    }
    return tracks;
}

std::vector<Album> SpotifyAPI::searchAlbum(const std::string &_query,
                                           const std::string &_market,
                                           unsigned int _limit,
                                           const std::string &_offset) {
    std::vector<Album> albums;
    for (auto &&jsonAlbum : search(ALBUM, _query, _market, _limit, _offset)) {
        albums.push_back(createAlbum(jsonAlbum));
    }
    return albums;
}

std::vector<Artist> SpotifyAPI::searchArtist(const std::string &_query,
                                             const std::string &_market,
                                             unsigned int _limit,
                                             const std::string &_offset) {
    std::vector<Artist> artists;
    for (auto &&jsonArtist : search(ARTIST, _query, _market, _limit, _offset)) {
        artists.push_back(createArtist(jsonArtist));
    }
    return artists;
}

std::vector<Playlist> SpotifyAPI::searchPlaylist(const std::string &_query,
                                                 const std::string &_market,
                                                 unsigned int _limit,
                                                 const std::string &_offset) {
    std::vector<Playlist> playlists;
    for (auto &&jsonPlaylist :
         search(PLAYLIST, _query, _market, _limit, _offset)) {
        if (jsonPlaylist == nullptr)
            continue;
        playlists.push_back(createPlaylist(jsonPlaylist));
    }
    return playlists;
}

/**
 * @brief
 *
 * @param _filename
 * @return std::string
 */
std::string SpotifyAPI::searchId(const std::string &_filename) {
    TagLib::FileRef file(_filename.c_str());
    if (!file.isNull()) {
        std::cout << "Filename: " << file.file()->name() << std::endl;
    } else {
        std::cerr << "Error: Invalid file!" << std::endl;
    }
    std::string title;
    std::string artist;
    std::string query;

    if (file.tag()) {
        title = file.tag()->title().toCString();
        artist = file.tag()->artist().toCString();
    }

    if (title.empty()) {
        query = _filename;
    } else if (artist.empty()) {
        query = title;
    } else {
        query = artist + " - " + title;
    }
    std::vector<Track> tracks = searchTrack(query);

    double bestScore = 0.0;
    std::vector<Track *> topMatches;
    int num_res(0);
    double allScores;
    for (auto &&track : tracks) {
        num_res++;
        double score;
        if (!title.empty()) {
            score = similarityScore(title, track.get_name());
        } else {
            score = similarityScore(title, _filename);
        }
        // Bonus, if tracklength matches
        score += similarityScore(file.audioProperties()->lengthInSeconds(),
                                 track.get_durationMs() / 1000);
        // Bonus, if album is single
        if (track.get_album().get_type() == "single") {
            score += 1;
        }
        std::cout << track.get_id() << "\tscore: " << score << std::endl;

        if (score > bestScore) {
            bestScore = score;
            topMatches.clear();
            topMatches.push_back(&track);
        } else if (score == bestScore) {
            topMatches.push_back(&track);
        }
        allScores += score;
    }
    double middle = allScores / num_res;

    // std::string image = "cover.jpg";

    // If only one top match, return it
    // if (topMatches.size() == 1) {
    std::cout << "Mean deviation: " << bestScore - middle << std::endl;

    return topMatches[0]->get_id();
    // }

    // std::cout << "length" <<
    // file.audioProperties()->lengthInMilliseconds()
    //           << std::endl;

    // if (_track.hasID3v1Tag()) {
    //     auto ID3v1Tag = _track.ID3v1Tag();
    // }
    // if (_track.hasID3v2Tag()) {
    //     auto ID3v2Tag = _track.ID3v2Tag();
    // }

    return "";
}

void SpotifyAPI::loadAdditionalData(Track &_track) {
    json jsonFullAlbum =
        headerRequest(urlAPI + "albums/" + _track.get_album().get_id());
    _track.get_album().set_copyright(jsonFullAlbum.at("copyrights")[0]["text"]);
    _track.get_album().set_label(jsonFullAlbum.at("label"));
}

void SpotifyAPI::verifyTags(const std::string &_filename) {

    Track spotifyTrack = getTrack(searchId(_filename));

    spotifyTrack.verifyTags(_filename);
}

} // namespace Spotify