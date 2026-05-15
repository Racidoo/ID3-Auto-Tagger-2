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

bool SpotifyAPI::insertTracklist(Album &_album, const json &_jsonAlbum) {
    if (!_jsonAlbum.contains("tracks") ||
        _jsonAlbum["tracks"]["items"].empty()) {
        return false;
    }
    std::vector<Track> tracks;
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
    _album.set_tracklist(tracks);
    return true;
}

bool SpotifyAPI::insertLabel(Album &_album, const json &_jsonAlbum) {
    if (!_jsonAlbum.contains("label")) {
        return false;
    }
    _album.set_label(_jsonAlbum["label"].get<std::string>());
    return true;
}

bool SpotifyAPI::insertCopyright(Album &_album, const json &_jsonAlbum) {
    if (_jsonAlbum.contains("copyrights")) {
        return false;
    }
    if (!_jsonAlbum["copyrights"].empty()) {
        _album.set_copyright(
            _jsonAlbum["copyright"][0]["text"].get<std::string>());
    }
    return true;
}

Album SpotifyAPI::createAlbum(const json &_jsonAlbum, bool _fullTags) {
    Album album(_jsonAlbum.at("id").get<std::string>(),
                _jsonAlbum.at("name").get<std::string>(), Album::State::Partial,
                _jsonAlbum.at("album_type").get<std::string>(),
                _jsonAlbum.at("total_tracks").get<unsigned int>(),
                _jsonAlbum.at("release_date").get<std::string>(),
                _jsonAlbum.at("images").at(1).at("url").get<std::string>(),
                createArtists(_jsonAlbum.at("artists")));
    if (insertTracklist(album, _jsonAlbum) && insertLabel(album, _jsonAlbum) &&
        insertCopyright(album, _jsonAlbum)) {
        album.set_state(Album::State::Full);
    }
    return album;
}

Artist SpotifyAPI::createArtist(const json &_jsonArtist) const {
    return Artist(
        _jsonArtist.at("id").get<std::string>(),
        _jsonArtist.at("name").get<std::string>(), Artist::State::Partial,
        _jsonArtist.contains("images") && !_jsonArtist.at("images").empty()
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
                    Playlist::State::Partial,
                    _jsonPlaylist.at("images")[0].at("url").get<std::string>(),
                    createUser(_jsonPlaylist.at("owner")));
}

Track SpotifyAPI::createTrack(const json &_jsonTrack,
                              const Album &_album) const {
    return Track(_jsonTrack.at("id").get<std::string>(),
                 _jsonTrack.at("name").get<std::string>(),
                 Track::State::Partial,
                 _jsonTrack.at("disc_number").get<unsigned int>(),
                 _jsonTrack.at("duration_ms").get<unsigned long>(),
                 _jsonTrack.at("explicit").get<bool>(),
                 _jsonTrack.at("track_number").get<unsigned int>(), _album,
                 createArtists(_jsonTrack["artists"]));
}

User SpotifyAPI::createUser(const json &_jsonUser) const {
    return User(_jsonUser.at("id").get<std::string>(),
                _jsonUser.at("display_name").get<std::string>(),
                User::State::Partial,
                _jsonUser.contains("images")
                    ? _jsonUser.at("images")[1].at("url").get<std::string>()
                    : "");
}

Album SpotifyAPI::getAlbum(const std::string &_id) {
    return createAlbum(performRequest("/albums/" + _id));
}

Artist SpotifyAPI::getArtist(const std::string &_id) {
    return createArtist(performRequest("/artists/" + _id));
}

Playlist SpotifyAPI::getPlaylist(const std::string &_id) {
    return createPlaylist(performRequest("/playlists/" + _id));
}

Track SpotifyAPI::getTrack(const std::string &_id) {
    json jsonTrack = performRequest("/tracks/" + _id);
    json jsonAlbum = jsonTrack.at("album");

    return createTrack(jsonTrack, createAlbum(jsonAlbum));
}

std::vector<Track> SpotifyAPI::getAlbumTracks(const std::string &_id) {
    json jsonAlbum = performRequest("/albums/" + _id);
    json jsonTracks = jsonAlbum.at("tracks");
    Album album = createAlbum(jsonAlbum);
    std::vector<Track> tracks;

    for (auto &&jsonTrack : jsonTracks["items"]) {
        tracks.push_back(createTrack(jsonTrack, album));
    }
    return tracks;
}

std::vector<Track> SpotifyAPI::getPlaylistTracks(const std::string &_id) {
    json jsonPlaylist = performRequest("/playlists/" + _id);
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
    url << "/search?q=" << Query::urlEncode(_query) << "&type=" << typeStr;

    if (!_market.empty())
        url << "&market=" << _market;
    if (!_limit == 0)
        url << "&limit=" << _limit;
    if (!_offset.empty())
        url << "&offset=" << _offset;

    json result = performRequest(url.str());
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
std::string SpotifyAPI::searchId(const TrackSearchContext &_ctx) {
    if (_ctx.title.empty() && _ctx.artist.empty() && _ctx.filename.empty())
        return {};

    // TagLib::FileRef file(_filename.c_str());
    // if (!file.isNull()) {
    //     std::cout << "Filename: " << file.file()->name() << std::endl;
    // } else {
    //     std::cerr << "Error: Invalid file!" << std::endl;
    // }

    std::string query;

    if (_ctx.title.empty())
        query = _ctx.filename;
    else if (_ctx.artist.empty())
        query = _ctx.title;
    else
        query = _ctx.artist + " - " + _ctx.title;

    if (!_ctx.album.empty())
        query += " - " + _ctx.album;
    // auto title = _localData->get_title();
    // auto artist = _localData->get_artist();
    // auto album = _localData->get_album();
    // auto filename = _localData->get_id();

    // if (file.tag()) {
    //     title = file.tag()->title().toCString();
    //     artist = file.tag()->artist().toCString();
    // }

    // if (title.empty()) {
    //     query = filename;
    // } else if (artist.empty()) {
    //     query = title;
    // } else {
    //     query = artist + " - " + title;
    // }

    // if (!album.empty()) {
    //     query += " - " + album;
    // }

    std::vector<Track> tracks;

    if (isValidIdFormat(_ctx.filename)) {
        tracks.push_back(getTrack(_ctx.filename));
    } else {
        tracks = searchTrack(query, "", 10);
    }

    double bestScore = 0.0;
    std::vector<Track *> topMatches;
    int num_res(0);
    double allScores = 0.0;
    for (auto &&track : tracks) {
        num_res++;
        double score;
        score = similarityScore(_ctx.title, track.get_name());
        score += similarityScore(_ctx.durationSeconds,
                                 track.get_durationMs() / 1000);

        // Bonus, if album is single
        if (track.get_album().get_albumType() == "single") {
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

    std::cout << "Mean deviation: " << bestScore - middle << std::endl;

    if (bestScore < 1.4) {
        return "";
    }

    return topMatches[0]->get_id();

    return "";
}

void SpotifyAPI::load(IMediaEntity &_obj) {
    if (auto *album = dynamic_cast<Spotify::Album *>(&_obj))
        loadAdditionalData(*album);
    else if (auto *artist = dynamic_cast<Spotify::Artist *>(&_obj))
        loadAdditionalData(*artist);
    else if (auto *playlist = dynamic_cast<Spotify::Playlist *>(&_obj))
        loadAdditionalData(*playlist);
    else if (auto *track = dynamic_cast<Spotify::Track *>(&_obj))
        loadAdditionalData(*track);
}

void SpotifyAPI::loadAdditionalData(Album &_album) {
    json jsonFullAlbum = performRequest("/albums/" + _album.get_id());

    (void *)insertTracklist(_album, jsonFullAlbum);
    (void *)insertLabel(_album, jsonFullAlbum);
    (void *)insertCopyright(_album, jsonFullAlbum);
}

void SpotifyAPI::loadAdditionalData(Artist &_artist) {
    std::cerr << "Not yet implemented!" << std::endl;
}

void SpotifyAPI::loadAdditionalData(Playlist &_playlist) {
    std::cerr << "Not yet implemented!" << std::endl;
}

void SpotifyAPI::loadAdditionalData(Track &_track) {
    json jsonFullAlbum =
        performRequest("/albums/" + _track.get_album().get_id());

    (void *)insertLabel(_track.get_album(), jsonFullAlbum);
    (void *)insertCopyright(_track.get_album(), jsonFullAlbum);
}

std::shared_ptr<Track>
SpotifyAPI::researchTags(const TrackSearchContext &_ctx) {
    std::string trackUuid = searchId(_ctx);
    if (trackUuid.empty()) {
        return nullptr;
    }
    auto track = getTrack(trackUuid);
    loadAdditionalData(track);
    return std::make_shared<Track>(track);
}

bool SpotifyAPI::isValidIdFormat(const std::string &_id) {
    static const std::regex pattern("^[A-Za-z0-9]{22}$");
    return std::regex_match(_id, pattern);
}

} // namespace Spotify