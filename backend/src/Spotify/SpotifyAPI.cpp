#include "../include/Spotify/SpotifyAPI.h"

namespace Spotify {

SpotifyAPI::SpotifyAPI() : Query("Spotify") {}

SpotifyAPI::~SpotifyAPI() {}

json SpotifyAPI::handleRequest(const std::string &_request) {

    std::string response_data;
    auto curl(curl_easy_init());
    struct curl_slist *headers(nullptr);

    if (!curl) {
        std::cerr << "cURL not initialized!" << std::endl;
        return {};
    }

    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + getValidToken()).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Ensure headers are set
    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    } else {
        std::cerr << "Error: Headers not initialized!" << std::endl;
    }

    // Set the request URL
    curl_easy_setopt(curl, CURLOPT_URL, _request.c_str());
    // Set response handling
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Execute the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "cURL request failed: " << curl_easy_strerror(res)
                  << std::endl;
        return {};
    }
    if (curl)
        curl_easy_cleanup(curl);

    if (response_data.empty()) {
        std::cerr << "Error: Received empty response!" << std::endl;
        return {};
    }

    // Parse JSON response
    json response = json::parse(response_data, nullptr, false);
    if (response.is_discarded()) {
        std::cerr << "Error: Failed to parse JSON response!" << std::endl;
        return {};
    }
    if (response.contains("error")) {
        std::cerr << response.dump(4) << std::endl;
    }
    std::ofstream("response.json") << response.dump(4);
    return response;
}

Album SpotifyAPI::createAlbum(const json &_jsonAlbum, bool _fullTags) {
    Album album(_jsonAlbum.at("id").get<std::string>(),
                _jsonAlbum.at("name").get<std::string>(),
                _jsonAlbum.at("album_type").get<std::string>(),
                _jsonAlbum.at("total_tracks").get<unsigned int>(),
                _jsonAlbum.at("release_date").get<std::string>(),
                _jsonAlbum.at("images").at(1).at("url").get<std::string>(),
                createArtists(_jsonAlbum.at("artists")));
    if (_fullTags) {
        const std::string id = _jsonAlbum.at("id").get<std::string>();
        json jsonFullAlbum = handleRequest(urlAPI + "albums/" + id);
        album.set_copyright(jsonFullAlbum.at("copyrights")[0]["text"]);
        album.set_label(jsonFullAlbum.at("label"));
    }
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
    return createAlbum(handleRequest(urlAPI + "albums/" + _id));
}

Artist SpotifyAPI::getArtist(const std::string &_id) {
    return createArtist(handleRequest(urlAPI + "artists/" + _id));
}

Playlist SpotifyAPI::getPlaylist(const std::string &_id) {
    return createPlaylist(handleRequest(urlAPI + "playlists/" + _id));
}

Track SpotifyAPI::getTrack(const std::string &_id) {
    json jsonTrack = handleRequest(urlAPI + "tracks/" + _id);
    json jsonArtist = jsonTrack.at("artists");
    json jsonAlbum = jsonTrack.at("album");

    return createTrack(jsonTrack, createAlbum(jsonAlbum));
}

std::vector<Track> SpotifyAPI::getAlbumTracks(const std::string &_id) {
    json jsonAlbum = handleRequest(urlAPI + "albums/" + _id);
    json jsonTracks = jsonAlbum.at("tracks");
    Album album = createAlbum(jsonAlbum);
    std::vector<Track> tracks;

    for (auto &&jsonTrack : jsonTracks["items"]) {
        tracks.push_back(createTrack(jsonTrack, album));
    }
    return tracks;
}

std::vector<Track> SpotifyAPI::getPlaylistTracks(const std::string &_id) {
    json jsonPlaylist = handleRequest(urlAPI + "playlists/" + _id);
    json jsonTracks = jsonPlaylist.at("tracks");
    std::vector<Track> tracks;

    for (auto &&jsonPlaylistEntry : jsonTracks["items"]) {
        auto jsonTrack = jsonPlaylistEntry.at("track");
        tracks.push_back(
            createTrack(jsonTrack, createAlbum(jsonTrack.at("album"))));
    }
    return tracks;
}

json SpotifyAPI::search(searchItem_type _type, const std::string &_query,
                        const std::string &_market, const std::string &_limit,
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
    if (!_limit.empty())
        url << "&limit=" << _limit;
    if (!_offset.empty())
        url << "&offset=" << _offset;

    json result = handleRequest(url.str());
    return result.at(typeStr + 's').at("items"); // e.g., result["tracks"]
}

std::vector<Track> SpotifyAPI::searchTrack(const std::string &_query,
                                           const std::string &_market,
                                           const std::string &_limit,
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
                                           const std::string &_limit,
                                           const std::string &_offset) {
    std::vector<Album> albums;
    for (auto &&jsonAlbum : search(ALBUM, _query, _market, _limit, _offset)) {
        albums.push_back(createAlbum(jsonAlbum));
    }
    return albums;
}

std::vector<Artist> SpotifyAPI::searchArtist(const std::string &_query,
                                             const std::string &_market,
                                             const std::string &_limit,
                                             const std::string &_offset) {
    std::vector<Artist> artists;
    for (auto &&jsonArtist : search(ARTIST, _query, _market, _limit, _offset)) {
        artists.push_back(createArtist(jsonArtist));
    }
    return artists;
}

std::vector<Playlist> SpotifyAPI::searchPlaylist(const std::string &_query,
                                                 const std::string &_market,
                                                 const std::string &_limit,
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
 * @param _query
 * @param _type
 * @return std::string
 */
std::string SpotifyAPI::searchId(const std::string &_filename,
                                 const std::string &_type) {
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
    if (topMatches.size() == 1) {
        std::cout << "Mean deviation: " << bestScore - middle << std::endl;

        return topMatches[0]->get_id();
    }

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

} // namespace Spotify