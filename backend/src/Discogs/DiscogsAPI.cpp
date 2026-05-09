#include "Discogs/DiscogsAPI.h"

namespace Discogs {

DiscogsAPI::DiscogsAPI(const std::string &_accessToken)
    : accessToken(_accessToken) {
    saveCredentials();
}

void DiscogsAPI::saveCredentials() {
    json credentials = loadCredentials();
    credentials["OAuth"]["Discogs"]["auth_token"] = accessToken;
    saveCredentialsToFile(credentials);
}

std::string DiscogsAPI::generateAccessToken() {
    if (!accessToken.empty()) {
        return accessToken;
    }
    json credentials = loadCredentials();
    if (credentials["OAuth"]["Discogs"]["auth_token"].empty()) {
        std::cerr << "Please provide 'access token' for Discogs API"
                  << std::endl;
        return {};
    }
    tokenExpirationTime =
        std::chrono::steady_clock::time_point(std::chrono::seconds(0));
    accessToken =
        credentials["OAuth"]["Discogs"]["auth_token"].get<std::string>();
    return accessToken;
}

void DiscogsAPI::prepareHeaders(struct curl_slist *&_headers) {

    std::string auth = "Authorization: Discogs token=" + accessToken;

    _headers = curl_slist_append(_headers, auth.c_str());
    _headers = curl_slist_append(_headers, "User-Agent: ID3AutoTagger/1.0");
    _headers = curl_slist_append(_headers, "Content-Type: application/json");
}

std::string DiscogsAPI::prepareUrl(const std::string &_url) {
    return urlAPI + _url;
}

std::vector<SearchResult> DiscogsAPI::search(const SearchParams &_params) {
    std::stringstream ss;

    // Discogs::SearchResult currently only supports type 'release'!
    ss << "/database/search?type=release";

    auto append = [&](const std::string &_key, const std::string &_value) {
        ss << "&" << _key << "=" << urlEncode(_value);
    };

    auto appendString = [&](const std::string &_key,
                            const std::optional<std::string> &_value) {
        if (_value) {
            append(_key, *_value);
        }
    };

    auto appendInt = [&](const std::string &_key,
                         const std::optional<int> &_value) {
        if (_value) {
            append(_key, std::to_string(*_value));
        }
    };

    appendString("query", _params.query);
    appendString("type", _params.type);
    appendString("title", _params.title);
    appendString("release_title", _params.releaseTitle);
    appendString("credit", _params.credit);
    appendString("artist", _params.artist);
    appendString("anv", _params.anv);
    appendString("label", _params.label);
    appendString("genre", _params.genre);
    appendString("style", _params.style);
    appendString("country", _params.country);
    appendInt("year", _params.year);
    appendString("format", _params.format);
    appendString("catno", _params.catno);
    appendString("barcode", _params.barcode);
    appendString("track", _params.track);
    appendString("submitter", _params.submitter);
    appendString("contributor", _params.contributor);

    auto jsonResults = performRequest(ss.str());
    std::vector<SearchResult> searchResults;
    for (auto &&result : jsonResults["results"]) {
        searchResults.emplace_back(
            result.at("id").get<int>(), result.value("title", ""),
            result.value("cover_image", ""), result.value("master_id", 0),
            parseGenres(result), parseStyles(result), result.value("year", ""));
    }
    return searchResults;
}

std::vector<MasterRelease>
DiscogsAPI::searchMasterRelease(const SearchParams &_params) {

    auto results = search(_params);

    if (results.empty())
        return {};

    std::vector<MasterRelease> releases;
    std::unordered_set<int> masterIds;
    for (auto &&result : results) {
        int id = std::stoi(result.get_id());
        if (id == 0) {
            continue;
        }
        masterIds.insert(id);
    }
    for (auto &&masterId : masterIds) {
        releases.push_back(std::move(getMasterRelease(masterId)));
    }
    return releases;
}

std::vector<Release> DiscogsAPI::searchRelease(const SearchParams &_params) {

    auto results = search(_params);

    if (results.empty())
        return {};

    std::unordered_set<int> masterIds;

    for (auto &&result : results) {

        int id = result.masterId;
        if (id != 0) {
            masterIds.insert(id);
        }
    }

    if (masterIds.size() == 1) {
        auto masterRelease = getMasterRelease(*masterIds.begin());
        return {Release(
            std::stoi(masterRelease.get_id()), masterRelease.get_name(),
            masterRelease.get_imageUrl(), masterRelease.get_artists(), "",
            masterRelease.get_genres(), {}, "", masterRelease.get_styles(),
            masterRelease.get_tracklist(), masterRelease.get_year(),
            masterRelease.get_videos(), true)};
    }

    std::vector<Release> verifiedReleases;
    std::vector<Release> fallbackReleases;

    for (auto &&result : results) {

        auto id = std::stoi(result.get_id());
        auto release = getRelease(id);

        if (release.isVerified()) {
            verifiedReleases.push_back(std::move(release));
        } else {
            fallbackReleases.push_back(std::move(release));
        }
    }

    if (!verifiedReleases.empty()) {
        return verifiedReleases;
    }

    return fallbackReleases;
}

Release DiscogsAPI::getRelease(int _releaseId, const std::string &_currAbbr) {
    return createRelease(performRequest(
        "/releases/" + std::to_string(_releaseId) + "?" + _currAbbr));
}

MasterRelease DiscogsAPI::getMasterRelease(int _masterId) {
    return createMasterRelease(
        performRequest("/masters/" + std::to_string(_masterId)));
}

std::vector<Artist> DiscogsAPI::createArtists(const json &_jsonArtists) {
    std::vector<Artist> artists;
    for (auto &&jsonArtist : _jsonArtists) {
        artists.emplace_back(jsonArtist.at("id").get<int>(),
                             jsonArtist.at("name").get<std::string>(),
                             jsonArtist.at("thumbnail_url").get<std::string>());
    }
    return artists;
}

std::vector<Label> DiscogsAPI::createLabels(const json &_jsonLabels) {
    std::vector<Label> labels;
    for (auto &&jsonLabel : _jsonLabels) {
        labels.emplace_back(jsonLabel.at("id").get<int>(),
                            jsonLabel.at("name").get<std::string>(),
                            jsonLabel.at("thumbnail_url").get<std::string>());
    }
    return labels;
}

MasterRelease DiscogsAPI::createMasterRelease(const json &_jsonRelease) {
    return MasterRelease(
        _jsonRelease.at("id").get<int>(),
        _jsonRelease.at("title").get<std::string>(),
        parseImageUrl(_jsonRelease), createArtists(_jsonRelease.at("artists")),
        parseGenres(_jsonRelease), parseStyles(_jsonRelease),
        parseTracklist(_jsonRelease), _jsonRelease.at("year").get<int>(),
        parseVideos(_jsonRelease));
}

Release DiscogsAPI::createRelease(const json &_jsonRelease) {
    return Release(
        _jsonRelease.at("id").get<int>(),
        _jsonRelease.at("title").get<std::string>(),
        parseImageUrl(_jsonRelease), createArtists(_jsonRelease.at("artists")),
        _jsonRelease.at("country").is_null() ? "" : _jsonRelease.at("country"),
        parseGenres(_jsonRelease), createLabels(_jsonRelease.at("labels")),
        _jsonRelease.at("released").get<std::string>(),
        parseStyles(_jsonRelease), parseTracklist(_jsonRelease),
        _jsonRelease.at("year").get<int>(), parseVideos(_jsonRelease),
        _jsonRelease.at("data_quality").get<std::string>() == "Correct");
}

std::vector<std::string> DiscogsAPI::parseGenres(const json &_j) {
    std::vector<std::string> genres;
    for (auto &&jsonGenre : _j.at("genre")) {
        genres.push_back(jsonGenre.get<std::string>());
    }
    return genres;
}
std::vector<std::string> DiscogsAPI::parseStyles(const json &_j) {
    std::vector<std::string> styles;
    for (auto &&jsonStyle : _j.at("style")) {
        styles.push_back(jsonStyle.get<std::string>());
    }
    return styles;
}

std::vector<Release::Track> DiscogsAPI::parseTracklist(const json &_j) {
    std::vector<Release::Track> tracklist;
    for (auto &&jsonTrack : _j.at("tracklist")) {
        tracklist.push_back({jsonTrack.at("duration").get<std::string>(),
                             jsonTrack.at("position").get<std::string>(),
                             jsonTrack.at("title").get<std::string>()});
    }
    return tracklist;
}

std::vector<Release::Video> DiscogsAPI::parseVideos(const json &_j) {
    std::vector<Release::Video> videos;
    for (auto &&jsonVideo : _j.at("videos")) {
        videos.push_back({jsonVideo.at("description").get<std::string>(),
                          jsonVideo.at("title").get<std::string>(),
                          jsonVideo.at("uri").get<std::string>(),
                          jsonVideo.at("duration").get<int>()});
    }
    return videos;
}

std::string DiscogsAPI::parseImageUrl(const json &_j) {
    if (auto it = _j.find("images"); it != _j.end() && !it->empty())
        return (*it)[0].value("resource_url", "");
    return {};
}

} // namespace Discogs