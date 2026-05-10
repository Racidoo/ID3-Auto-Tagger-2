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

ISearchResult DiscogsAPI::search(const SearchParams &_params) {
    std::stringstream ss;
    bool firstArgument(true);

    // Discogs::SearchResult currently only supports type 'release'!
    ss << "/database/search?";

    auto append = [&](const std::string &_key, const std::string &_value) {
        if (!firstArgument) {
            ss << "&";
            firstArgument = false;
        }
        ss << _key << "=" << urlEncode(_value);
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
    ISearchResult searchResult;
    for (auto &&result : jsonResults["results"]) {

        if (!result.contains("type") || result["type"].is_null() ||
            result["type"].empty())
            continue;

        const auto &type = result["type"].get<std::string>();

        if (type == "release") {
            searchResult.albums.push_back(
                IAlbum::fromDiscogs(createRelease(result)));
        } else if (type == "master") {
            searchResult.albums.push_back(
                IAlbum::fromDiscogs(createMasterRelease(result)));
        } else if (type == "artist") {
            bool fallbackUsed(false);
            searchResult.artists.push_back(
                IArtist::fromDiscogs(std::make_shared<Artist>(
                    createArtistFromArtist(result, fallbackUsed))));
        } else if (type == "label") {
            // currently not supported
            continue;
        } else {
            continue;
        }

        // searchResults.emplace_back(
        //     result.at("id").get<int>(), result.value("title", ""),
        //     Release::MetadataState::Preview, result.value("cover_image", ""),
        //     result.value("master_id", 0), parseGenres(result),
        //     parseStyles(result), result.value("year", ""));
    }
    return searchResult;
}

// std::vector<MasterRelease>
// DiscogsAPI::searchMasterRelease(const SearchParams &_params) {

//     auto results = search(_params);

//     if (results.empty())
//         return {};

//     std::vector<MasterRelease> releases;
//     std::unordered_set<int> masterIds;
//     for (auto &&result : results) {
//         int id = std::stoi(result.get_id());
//         if (id == 0) {
//             continue;
//         }
//         masterIds.insert(id);
//     }
//     for (auto &&masterId : masterIds) {
//         releases.push_back(std::move(getMasterRelease(masterId)));
//     }
//     return releases;
// }

std::vector<std::shared_ptr<Release>>
DiscogsAPI::searchRelease(const SearchParams &_params) {

    auto results = search(_params);

    if (results.albums.empty())
        return {};

    std::unordered_set<int> masterIds;

    for (auto &&result : results.albums) {

        int id = result->get_discogsRelease()->get_masterId();
        if (id != 0) {
            masterIds.insert(id);
        }
    }

    if (masterIds.size() == 1) {
        auto masterRelease = getMasterRelease(*masterIds.begin());
        auto release = Release(
            std::stoi(masterRelease->get_id()), masterRelease->get_name(),
            Release::MetadataState::Partial, masterRelease->get_imageUrl(),
            masterRelease->get_artists(), std::stoi(masterRelease->get_id()),
            "", masterRelease->get_genres(), {}, "",
            masterRelease->get_styles(), masterRelease->get_tracklist(),
            masterRelease->get_year(), masterRelease->get_videos(), true);
        return {std::make_shared<Release>(release)};
    }

    std::vector<std::shared_ptr<Release>> verifiedReleases;
    std::vector<std::shared_ptr<Release>> fallbackReleases;

    for (auto &&result : results.albums) {

        auto id = std::stoi(result->get_id());
        auto release = getRelease(id);

        if (release->isVerified()) {
            verifiedReleases.push_back(release);
        } else {
            fallbackReleases.push_back(release);
        }
    }

    if (!verifiedReleases.empty()) {
        return verifiedReleases;
    }

    return fallbackReleases;
}

std::shared_ptr<Release> DiscogsAPI::getRelease(int _releaseId,
                                                const std::string &_currAbbr) {
    return createRelease(performRequest(
        "/releases/" + std::to_string(_releaseId) + "?" + _currAbbr));
}

std::shared_ptr<MasterRelease> DiscogsAPI::getMasterRelease(int _masterId) {
    return createMasterRelease(
        performRequest("/masters/" + std::to_string(_masterId)));
}

Artist DiscogsAPI::createArtistFromArtist(const json &_jsonArtist,
                                          bool &_fallbackUsed) {
    return Artist(_jsonArtist.at("id").get<int>(),
                  _jsonArtist.at("title").get<std::string>(),
                  Artist::MetadataState::Full,
                  Release::getOptional<std::string>(_jsonArtist, "cover_image",
                                                    {}, _fallbackUsed));
}

Artist DiscogsAPI::createArtistFromRelease(const json &_jsonArtist,
                                           bool &_fallbackUsed) {
    return Artist(_jsonArtist.at("id").get<int>(),
                  _jsonArtist.at("name").get<std::string>(),
                  Artist::MetadataState::Preview,
                  Release::getOptional<std::string>(
                      _jsonArtist, "thumbnail_url", {}, _fallbackUsed));
}

std::vector<Artist> DiscogsAPI::createArtists(const json &_jsonArtists,
                                              bool &_fallbackUsed) {
    std::vector<Artist> artists;
    for (auto &&jsonArtist : _jsonArtists) {
        artists.push_back(createArtistFromRelease(jsonArtist, _fallbackUsed));
    }
    return artists;
}

std::vector<Label> DiscogsAPI::createLabels(const json &_jsonLabels,
                                            bool &_fallbackUsed) {
    std::vector<Label> labels;
    for (auto &&jsonLabel : _jsonLabels) {
        labels.emplace_back(jsonLabel.at("id").get<int>(),
                            jsonLabel.at("name").get<std::string>(),
                            Label::MetadataState::Partial,
                            jsonLabel.at("thumbnail_url").get<std::string>());
    }
    return labels;
}

std::shared_ptr<MasterRelease>
DiscogsAPI::createMasterRelease(const json &_jsonRelease) {
    bool fallbackUsed(false);
    auto masterRelease = std::make_shared<MasterRelease>(
        _jsonRelease.at("id").get<int>(),
        _jsonRelease.at("title").get<std::string>(),
        MasterRelease::MetadataState::Full,
        MasterRelease::getOptional<std::string>(_jsonRelease, "cover_image", {},
                                                fallbackUsed),
        createArtists(MasterRelease::getOptional<json>(_jsonRelease, "artists",
                                                       {}, fallbackUsed),
                      fallbackUsed),
        parseGenres(_jsonRelease, fallbackUsed),
        parseStyles(_jsonRelease, fallbackUsed),
        parseTracklist(_jsonRelease, fallbackUsed),
        std::stoi(MasterRelease::getOptional<std::string>(_jsonRelease, "year",
                                                          "0", fallbackUsed)),
        parseVideos(_jsonRelease, fallbackUsed));
    if (fallbackUsed) {
        masterRelease->set_state(MasterRelease::MetadataState::Partial);
    }
    return masterRelease;
}

std::shared_ptr<Release> DiscogsAPI::createRelease(const json &_jsonRelease) {
    bool fallbackUsed(false);
    auto release = std::make_shared<Release>(
        _jsonRelease.at("id").get<int>(),
        _jsonRelease.at("title").get<std::string>(),
        Release::MetadataState::Full,
        Release::getOptional<std::string>(_jsonRelease, "cover_image", {},
                                          fallbackUsed),
        createArtists(Release::getOptional<json>(_jsonRelease, "artists", {},
                                                 fallbackUsed),
                      fallbackUsed),
        Release::getOptional<int>(_jsonRelease, "master_id", 0, fallbackUsed),
        Release::getOptional<std::string>(_jsonRelease, "country", "",
                                          fallbackUsed),
        parseGenres(_jsonRelease, fallbackUsed),
        createLabels(Release::getOptional<json>(_jsonRelease, "labels", {},
                                                fallbackUsed),
                     fallbackUsed),
        Release::getOptional<std::string>(_jsonRelease, "released", "",
                                          fallbackUsed),
        parseStyles(_jsonRelease, fallbackUsed),
        parseTracklist(_jsonRelease, fallbackUsed),
        std::stoi(Release::getOptional<std::string>(_jsonRelease, "year", "0",
                                                    fallbackUsed)),
        parseVideos(_jsonRelease, fallbackUsed),
        Release::getOptional<std::string>(_jsonRelease, "data_quality", "",
                                          fallbackUsed) == "Correct");
    if (fallbackUsed) {
        release->set_state(MasterRelease::MetadataState::Partial);
    }
    return release;
}

std::vector<std::string> DiscogsAPI::parseGenres(const json &_j,
                                                 bool &_fallbackUsed) {
    std::vector<std::string> genres;
    for (auto &&jsonGenre :
         Release::getOptional<json>(_j, "genre", {}, _fallbackUsed)) {
        genres.push_back(jsonGenre.get<std::string>());
    }
    return genres;
}
std::vector<std::string> DiscogsAPI::parseStyles(const json &_j,
                                                 bool &_fallbackUsed) {
    std::vector<std::string> styles;
    for (auto &&jsonStyle :
         Release::getOptional<json>(_j, "style", {}, _fallbackUsed)) {
        styles.push_back(jsonStyle.get<std::string>());
    }
    return styles;
}

std::vector<Release::Track> DiscogsAPI::parseTracklist(const json &_j,
                                                       bool &_fallbackUsed) {
    std::vector<Release::Track> tracklist;
    for (auto &&jsonTrack :
         Release::getOptional<json>(_j, "tracklist", {}, _fallbackUsed)) {
        tracklist.push_back({Release::getOptional<std::string>(
                                 _j, "duration", {}, _fallbackUsed),
                             Release::getOptional<std::string>(
                                 _j, "position", {}, _fallbackUsed),
                             Release::getOptional<std::string>(_j, "title", {},
                                                               _fallbackUsed)});
    }
    return tracklist;
}

std::vector<Release::Video> DiscogsAPI::parseVideos(const json &_j,
                                                    bool &_fallbackUsed) {
    std::vector<Release::Video> videos;
    for (auto &&jsonVideo :
         Release::getOptional<json>(_j, "videos", {}, _fallbackUsed)) {
        videos.push_back(
            {Release::getOptional<std::string>(_j, "description", {},
                                               _fallbackUsed),
             Release::getOptional<std::string>(_j, "title", {}, _fallbackUsed),
             Release::getOptional<std::string>(_j, "uri", {}, _fallbackUsed),
             Release::getOptional<int>(_j, "duration", {}, _fallbackUsed)

            });
    }
    return videos;
}

std::string DiscogsAPI::parseImageUrl(const json &_j, bool &_fallbackUsed) {
    if (auto it = _j.find("images"); it != _j.end() && !it->empty())
        return (*it)[0].value("resource_url", "");
    _fallbackUsed = true;
    return {};
}

} // namespace Discogs