#include "../include/Spotify/SpotifyAPI.h"

namespace Spotify {

SpotifyAPI::SpotifyAPI() {}
SpotifyAPI::SpotifyAPI(const std::string &_clientId,
                       const std::string &_clientSecret)
    : clientId(_clientId), clientSecret(_clientSecret) {
    saveCredentials();
}

void SpotifyAPI::saveCredentials() {
    json credentials = loadCredentials();
    credentials["OAuth"]["Spotify"]["client_id"] = clientId;
    credentials["OAuth"]["Spotify"]["client_secret"] = clientSecret;
    credentials["OAuth"]["Spotify"]["token_uri"] =
        "https://accounts.spotify.com/api/token";

    saveCredentialsToFile(credentials);
}

std::string SpotifyAPI::generateAccessToken() {

    if (!accessToken.empty() &&
        std::chrono::steady_clock::now() < tokenExpirationTime) {
        return accessToken;
    }

    json credentials = loadCredentials();

    auto &spotify = credentials["OAuth"]["Spotify"];

    if (spotify["client_id"].empty() || spotify["client_secret"].empty()) {
        std::cerr
            << "Please provide 'client_id' and 'client_secret' from Spotify API"
            << std::endl;
        return "";
    }
    if (spotify["token_uri"].empty()) {
        std::cerr << "Please provide 'token_uri' from Spotify API" << std::endl;
        return "";
    }

    auto clientId = spotify["client_id"].get<std::string>();
    auto clientSecret = spotify["client_secret"].get<std::string>();
    static std::string tokenUri = spotify["token_uri"].get<std::string>();

    CURL *accessTokenCurl = curl_easy_init();
    if (!accessTokenCurl) {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return "";
    }

    std::string response_data;
    std::string postFields =
        "grant_type=client_credentials&client_id=" + clientId +
        "&client_secret=" + clientSecret;

    struct curl_slist *accessTokenHeaders = nullptr;
    accessTokenHeaders = curl_slist_append(
        accessTokenHeaders, "Content-Type: application/x-www-form-urlencoded");

    // Set cURL options
    curl_easy_setopt(accessTokenCurl, CURLOPT_URL, tokenUri.c_str());
    curl_easy_setopt(accessTokenCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(accessTokenCurl, CURLOPT_POSTFIELDS, postFields.c_str());
    curl_easy_setopt(accessTokenCurl, CURLOPT_HTTPHEADER, accessTokenHeaders);
    curl_easy_setopt(accessTokenCurl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(accessTokenCurl, CURLOPT_WRITEDATA, &response_data);

    CURLcode res = curl_easy_perform(accessTokenCurl);
    curl_easy_cleanup(accessTokenCurl);
    curl_slist_free_all(accessTokenHeaders);

    if (res != CURLE_OK) {
        std::cerr << "cURL request failed: " << curl_easy_strerror(res)
                  << std::endl;
        return "";
    }

    // Parse JSON response
    json response_json = json::parse(response_data);
    if (response_json.contains("access_token")) {
        tokenExpirationTime =
            std::chrono::steady_clock::now() +
            std::chrono::seconds(response_json.at("expires_in").get<int>());
        accessToken = response_json["access_token"];
    } else {
        std::cerr << "Failed to retrieve access token! Response:" +
                         response_json.dump(4)
                  << std::endl;
    }
    return accessToken;
}

void SpotifyAPI::prepareHeaders(struct curl_slist *&_headers) {

    _headers = curl_slist_append(
        _headers, ("Authorization: Bearer " + generateAccessToken()).c_str());

    _headers = curl_slist_append(_headers, "Content-Type: application/json");
}

std::string SpotifyAPI::prepareUrl(const std::string &_url) {
    return urlAPI + _url;
}

bool SpotifyAPI::insertTracklist(std::shared_ptr<Album> _album,
                                 const json &_jsonAlbum) {
    if (!_jsonAlbum.contains("tracks") ||
        _jsonAlbum["tracks"]["items"].empty()) {
        return false;
    }
    std::vector<std::shared_ptr<ITrack>> tracks;
    // ToDo: Tracklimit is capped at 20. Implement loadMore()
    if (_jsonAlbum["tracks"]["total"].get<int>() >
        _jsonAlbum["tracks"]["limit"].get<int>()) {
        std::cout << "[" << __FILE__ << ":" << __LINE__
                  << "]: Loading additional tracks is currently not supported"
                  << std::endl;
    }
    for (auto &&jsonTrack : _jsonAlbum["tracks"]["items"]) {
        tracks.push_back(createTrack(jsonTrack, _album));
    }
    _album->set_tracklist(tracks);
    return true;
}

bool SpotifyAPI::insertLabel(std::shared_ptr<Album> _album,
                             const json &_jsonAlbum) {
    if (!_jsonAlbum.contains("label")) {
        return false;
    }
    _album->set_label(_jsonAlbum["label"].get<std::string>());
    return true;
}

bool SpotifyAPI::insertCopyright(std::shared_ptr<Album> _album,
                                 const json &_jsonAlbum) {
    if (!_jsonAlbum.contains("copyrights")) {
        return false;
    }
    if (!_jsonAlbum["copyrights"].empty()) {
        _album->set_copyright(
            _jsonAlbum["copyrights"][0]["text"].get<std::string>());
    }
    return true;
}

IAlbum::album_type_t SpotifyAPI::parseAlbumType(const json &_jsonAlbum) {
    const auto &stringType = _jsonAlbum.at("album_type").get<std::string>();
    if (stringType == "single") {
        return IAlbum::album_type_t::SINGLE;
    }
    if (stringType == "album") {
        return IAlbum::album_type_t::ALBUM;
    }
    if (stringType == "compilation") {
        return IAlbum::album_type_t::COMPILATION;
    }
    return IAlbum::album_type_t::UNDEFINED;
}

std::shared_ptr<Album> SpotifyAPI::createAlbum(const json &_jsonAlbum,
                                                bool _fullTags) {
    auto album = std::make_shared<Album>(
        _jsonAlbum.at("id").get<std::string>(),
        _jsonAlbum.at("name").get<std::string>(), Album::State::Partial,
        parseAlbumType(_jsonAlbum),
        _jsonAlbum.at("total_tracks").get<std::size_t>(),
        _jsonAlbum.at("release_date").get<std::string>(),
        std::stoi(
            _jsonAlbum.at("release_date").get<std::string>().substr(0, 4)),
        _jsonAlbum.at("images").at(1).at("url").get<std::string>(),
        createArtists(_jsonAlbum.at("artists")), this);
    if (insertTracklist(album, _jsonAlbum) && insertLabel(album, _jsonAlbum) &&
        insertCopyright(album, _jsonAlbum)) {
        album->set_state(Album::State::Full);
    }
    return album;
}

std::shared_ptr<IArtist> SpotifyAPI::createArtist(const json &_jsonArtist) {
    return std::make_shared<Artist>(
        _jsonArtist.at("id").get<std::string>(),
        _jsonArtist.at("name").get<std::string>(), Artist::State::Partial,
        _jsonArtist.contains("images") && !_jsonArtist.at("images").empty()
            ? _jsonArtist.at("images")[1].at("url").get<std::string>()
            : "",
        this);
}

std::vector<std::shared_ptr<IArtist>>
SpotifyAPI::createArtists(const json &_jsonArtists) {
    std::vector<std::shared_ptr<IArtist>> artists;
    for (auto &&artist : _jsonArtists) {
        artists.push_back(createArtist(artist));
    }
    return artists;
}

std::shared_ptr<IPlaylist>
SpotifyAPI::createPlaylist(const json &_jsonPlaylist) {
    return std::make_shared<Playlist>(
        _jsonPlaylist.at("id").get<std::string>(),
        _jsonPlaylist.at("name").get<std::string>(), Playlist::State::Partial,
        _jsonPlaylist.at("images")[0].at("url").get<std::string>(),
        createUser(_jsonPlaylist.at("owner")), this);
}

std::shared_ptr<ITrack>
SpotifyAPI::createTrack(const json &_jsonTrack,
                        std::shared_ptr<Album> _album) {
    return std::make_shared<Track>(
        _jsonTrack.at("id").get<std::string>(),
        _jsonTrack.at("name").get<std::string>(), Track::State::Partial,
        _jsonTrack.at("disc_number").get<std::size_t>(),
        _jsonTrack.at("duration_ms").get<unsigned long>() / 1000,
        _jsonTrack.at("explicit").get<bool>(),
        _jsonTrack.at("track_number").get<std::size_t>(), _album,
        createArtists(_jsonTrack["artists"]), this);
}

std::shared_ptr<User> SpotifyAPI::createUser(const json &_jsonUser) {
    return std::make_shared<User>(
        _jsonUser.at("id").get<std::string>(),
        _jsonUser.at("display_name").get<std::string>(), User::State::Partial,
        (_jsonUser.contains("images")
             ? _jsonUser.at("images")[1].at("url").get<std::string>()
             : ""),
        this);
}

std::shared_ptr<IAlbum> SpotifyAPI::getAlbum(const std::string &_id) {
    return createAlbum(performRequest("/albums/" + _id));
}

std::shared_ptr<IArtist> SpotifyAPI::getArtist(const std::string &_id) {
    return createArtist(performRequest("/artists/" + _id));
}

std::shared_ptr<IPlaylist> SpotifyAPI::getPlaylist(const std::string &_id) {
    return createPlaylist(performRequest("/playlists/" + _id));
}

std::shared_ptr<ITrack> SpotifyAPI::getTrack(const std::string &_id) {
    json jsonTrack = performRequest("/tracks/" + _id);
    json jsonAlbum = jsonTrack.at("album");

    return createTrack(jsonTrack, createAlbum(jsonAlbum));
}

std::vector<std::shared_ptr<ITrack>>
SpotifyAPI::getAlbumTracks(const std::string &_id) {
    json jsonAlbum = performRequest("/albums/" + _id);
    json jsonTracks = jsonAlbum.at("tracks");
    auto album = createAlbum(jsonAlbum);
    std::vector<std::shared_ptr<ITrack>> tracks;

    for (auto &&jsonTrack : jsonTracks["items"]) {
        tracks.push_back(createTrack(jsonTrack, album));
    }
    return tracks;
}

std::vector<std::shared_ptr<ITrack>>
SpotifyAPI::getPlaylistTracks(const std::string &_id) {
    json jsonPlaylist = performRequest("/playlists/" + _id);
    json jsonTracks = jsonPlaylist.at("tracks");
    std::vector<std::shared_ptr<ITrack>> tracks;

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
    url << "/search?q=" << Query::urlEncode(_query) << "&type=" << typeStr;

    if (!_market.empty())
        url << "&market=" << _market;
    if (!_limit == 0)
        url << "&limit=" << _limit;
    if (!_offset.empty())
        url << "&offset=" << _offset;

    json result = performRequest(url.str());
    if (result.contains("error")) {
        std::cout << result.dump(4) << std::endl;
        return {};
    }
    return result.at(typeStr + 's').at("items"); // e.g., result["tracks"]
}

std::vector<std::shared_ptr<ITrack>>
SpotifyAPI::searchTrack(const std::string &_query, const std::string &_market,
                        unsigned int _limit, const std::string &_offset) {
    std::vector<std::shared_ptr<ITrack>> tracks;
    for (auto &&jsonTrack : search(TRACK, _query, _market, _limit, _offset)) {
        tracks.push_back(
            createTrack(jsonTrack, createAlbum(jsonTrack.at("album"))));
    }
    return tracks;
}

std::vector<std::shared_ptr<IAlbum>>
SpotifyAPI::searchAlbum(const std::string &_query, const std::string &_market,
                        unsigned int _limit, const std::string &_offset) {
    std::vector<std::shared_ptr<IAlbum>> albums;
    for (auto &&jsonAlbum : search(ALBUM, _query, _market, _limit, _offset)) {
        albums.push_back(createAlbum(jsonAlbum));
    }
    return albums;
}

std::vector<std::shared_ptr<IArtist>>
SpotifyAPI::searchArtist(const std::string &_query, const std::string &_market,
                         unsigned int _limit, const std::string &_offset) {
    std::vector<std::shared_ptr<IArtist>> artists;
    for (auto &&jsonArtist : search(ARTIST, _query, _market, _limit, _offset)) {
        artists.push_back(createArtist(jsonArtist));
    }
    return artists;
}

std::vector<std::shared_ptr<IPlaylist>>
SpotifyAPI::searchPlaylist(const std::string &_query,
                           const std::string &_market, unsigned int _limit,
                           const std::string &_offset) {
    std::vector<std::shared_ptr<IPlaylist>> playlists;
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
// std::string SpotifyAPI::searchId(const TrackSearchContext &_ctx) {
//     if (_ctx.title.empty() && _ctx.artist.empty() && _ctx.filename.empty())
//         return {};

//     std::string query;

//     if (_ctx.title.empty())
//         query = _ctx.filename;
//     else if (_ctx.artist.empty())
//         query = _ctx.title;
//     else
//         query = _ctx.artist + " - " + _ctx.title;

//     if (!_ctx.album.empty())
//         query += " - " + _ctx.album;

//     std::vector<std::shared_ptr<ITrack>> tracks;

//     if (isValidIdFormat(_ctx.filename)) {
//         tracks.push_back(getTrack(_ctx.filename));
//     } else {
//         tracks = searchTrack(query, "", 10);
//     }

//     double bestScore = 0.0;
//     std::vector<std::shared_ptr<ITrack>> topMatches;
//     int num_res(0);
//     double allScores = 0.0;
//     for (auto &&track : tracks) {
//         num_res++;
//         double score;
//         score = similarityScore(_ctx.title, track->get_name());
//         score += similarityScore(_ctx.durationSeconds, track->get_length());

//         // Bonus, if album is single
//         if (std::dynamic_pointer_cast<Track>(track)->get_album()->get_type()
//         ==
//             "single") {
//             score += 1;
//         }
//         std::cout << track->get_id() << "\tscore: " << score << std::endl;

//         if (score > bestScore) {
//             bestScore = score;
//             topMatches.clear();
//             topMatches.push_back(track);
//         } else if (score == bestScore) {
//             topMatches.push_back(track);
//         }
//         allScores += score;
//     }
//     double middle = allScores / num_res;

//     std::cout << "Mean deviation: " << bestScore - middle << std::endl;

//     if (bestScore < 1.4) {
//         return "";
//     }

//     return topMatches[0]->get_id();

//     return "";
// }

bool SpotifyAPI::load(std::shared_ptr<IMediaEntity> _obj) {
    if (auto album = std::dynamic_pointer_cast<Spotify::Album>(_obj))
        return loadAdditionalData(album);
    else if (auto artist = std::dynamic_pointer_cast<Spotify::Artist>(_obj))
        return loadAdditionalData(artist);
    else if (auto playlist = std::dynamic_pointer_cast<Spotify::Playlist>(_obj))
        return loadAdditionalData(playlist);
    else if (auto track = std::dynamic_pointer_cast<Spotify::Track>(_obj))
        return loadAdditionalData(track);
    return false;
}

bool SpotifyAPI::loadAdditionalData(std::shared_ptr<Album> _album) {
    json jsonFullAlbum = performRequest("/albums/" + _album->get_id());
    return (insertTracklist(_album, jsonFullAlbum) &&
            insertLabel(_album, jsonFullAlbum) &&
            insertCopyright(_album, jsonFullAlbum));
}

bool SpotifyAPI::loadAdditionalData(std::shared_ptr<Artist> _artist) {
    std::cerr << "Not yet implemented!" << std::endl;
    return false;
}

bool SpotifyAPI::loadAdditionalData(std::shared_ptr<Playlist> _playlist) {
    std::cerr << "Not yet implemented!" << std::endl;
    return false;
}

bool SpotifyAPI::loadAdditionalData(std::shared_ptr<Track> _track) {
    json jsonFullAlbum =
        performRequest("/albums/" + _track->get_album()->get_id());

    return insertLabel(std::dynamic_pointer_cast<Album>(_track->get_album()),
                       jsonFullAlbum) &&
           insertCopyright(
               std::dynamic_pointer_cast<Album>(_track->get_album()),
               jsonFullAlbum);
}

// std::shared_ptr<ITrack>
// SpotifyAPI::researchTags(const TrackSearchContext &_ctx) {
//     std::string trackUuid = searchId(_ctx);
//     if (trackUuid.empty()) {
//         return nullptr;
//     }
//     auto track = getTrack(trackUuid);
//     loadAdditionalData(std::dynamic_pointer_cast<Track>(track));
//     return track;
// }

bool SpotifyAPI::isValidIdFormat(const std::string &_id) {
    static const std::regex pattern("^[A-Za-z0-9]{22}$");
    return std::regex_match(_id, pattern);
}

} // namespace Spotify