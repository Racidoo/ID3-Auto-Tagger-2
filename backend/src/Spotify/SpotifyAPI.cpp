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
    return response;
}

json SpotifyAPI::searchTrack(const std::string &_query) {
    std::stringstream url;
    url << "https://api.spotify.com/v1/search?q="
        << curl_escape(_query.c_str(), 0) << "&type=track";
    return handleRequest(url.str());
}

json SpotifyAPI::search(searchItem_type _type, const std::string &_q,
                        const std::string &_market, const std::string &_limit,
                        const std::string &_offset) {
    return {};
}

Album SpotifyAPI::createAlbum(const json &_jsonAlbum, bool _fullTags) {
    std::vector<Artist> albumArtists;
    for (auto &&artist : _jsonAlbum.at("artists")) {
        albumArtists.emplace_back(artist.at("id").get<std::string>(),
                                  artist.at("name").get<std::string>());
    }

    Album album(_jsonAlbum.at("id").get<std::string>(),
                _jsonAlbum.at("name").get<std::string>(),
                _jsonAlbum.at("album_type").get<std::string>(),
                _jsonAlbum.at("total_tracks").get<unsigned int>(),
                _jsonAlbum.at("release_date").get<std::string>(),
                _jsonAlbum.at("images").at(1).at("url").get<std::string>(),
                albumArtists);
    if (_fullTags) {
        const std::string id = _jsonAlbum.at("id").get<std::string>();
        json jsonFullAlbum = handleRequest(urlAPI + "albums/" + id);
        album.set_copyright(jsonFullAlbum.at("copyrights")[0]["text"]);
        album.set_label(jsonFullAlbum.at("label"));
    }
    return album;
}

std::vector<Artist> SpotifyAPI::createArtists(const json &_jsonArtists) const {
    std::vector<Artist> trackArtists;
    for (auto &&artist : _jsonArtists) {
        trackArtists.emplace_back(artist.at("id").get<std::string>(),
                                  artist.at("name").get<std::string>());
    }
    return trackArtists;
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
    std::ofstream("response.json") << jsonPlaylist.dump(4);
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
 * @param _query
 * @param _type
 * @return std::string
 */
std::string SpotifyAPI::searchId(const std::string &_filename,
                                 const std::string &_type) {
    // std::string filepath =
    //     (std::filesystem::current_path() / _filename).string();
    // std::cout << filepath << std::endl;
    TagLib::FileRef file(_filename.c_str());
    if (!file.isNull()) {
        std::cout << "Filename: " << file.file()->name() << std::endl;
    } else {
        std::cerr << "Error: Invalid file!" << std::endl;
    }
    // auto file = _track.name();
    // std::string filename = _track.name();
    std::string title;
    std::string artist;
    // if (_track.isValid())
    //     std::cout << "Filename: " << _track.name().to8Bit(true) <<
    //     std::endl;
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

    // std::cout << "Query for Spotify API: " << query << std::endl;

    // std::ofstream outfile(std::filesystem::current_path() /
    // "response.json");
    json response = searchTrack(query);

    double bestScore = 0.0;
    std::vector<json *> topMatches;
    int num_res(0);
    double allScores;
    for (auto &&result : response["tracks"]["items"]) {
        num_res++;
        double score;
        if (!title.empty()) {
            score = similarityScore(title, result["name"]);
        } else {
            score = similarityScore(title, _filename);
        }
        // Bonus, if tracklength matches
        score += similarityScore(file.audioProperties()->lengthInSeconds(),
                                 result["duration_ms"].get<int>() / 1000);
        // Bonus, if album is single
        if (result["album"]["album_type"] == "single") {
            score += 1;
        }
        std::cout << result["id"] << "\tscore: " << score << std::endl;

        if (score > bestScore) {
            bestScore = score;
            topMatches.clear();
            topMatches.push_back(&result);
        } else if (score == bestScore) {
            topMatches.push_back(&result);
        }
        allScores += score;
    }
    double middle = allScores / num_res;

    std::string image = "cover.jpg";

    // If only one top match, return it
    if (topMatches.size() == 1) {
        std::cout << "Mean deviation: " << bestScore - middle << std::endl;
        // const std::string url = (*topMatches[0])
        //                             .at("album")
        //                             .at("images")
        //                             .at(1)
        //                             .at("url")
        //                             .get<std::string>();
        // downloadImage(url, image);

        return (*topMatches[0])["id"];
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