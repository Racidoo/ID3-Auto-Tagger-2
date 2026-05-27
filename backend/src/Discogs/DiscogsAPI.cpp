#include "Discogs/DiscogsAPI.h"
#include "Discogs/Artist.h"
#include "Discogs/Label.h"
#include "Discogs/Release.h"
#include "Discogs/ReleaseTrack.h"

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

ISearchResult DiscogsAPI::search(const SearchParams &_params,
                                 std::size_t _perPage, std::size_t _page) {
    if (_params.categories.empty()) {
        std::cerr << "Empty search categories. Will not query the Discogs-API!"
                  << std::endl;
    }

    std::stringstream ss;

    ss << "/database/search?";

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

    if (_perPage != 0) {
        appendInt("per_page", _perPage);
    }
    if (_page != 0) {
        appendInt("page", _page);
    }

    auto jsonResults = performRequest(ss.str());
    ISearchResult searchResult;
    for (auto &&result : jsonResults["results"]) {

        if (!result.contains("type") || result["type"].is_null() ||
            result["type"].empty())
            continue;

        const auto &type = result["type"].get<std::string>();

        if ((type == "release" || type == "master") &&
            _params.categories.find(ISearchResult::SearchCategory::Album) !=
                _params.categories.end()) {
            searchResult.albums.push_back(createRelease(result));
        } else if (type == "artist" &&
                   _params.categories.find(
                       ISearchResult::SearchCategory::Artist) !=
                       _params.categories.end()) {
            bool fallbackUsed(false);
            searchResult.artists.push_back(
                createArtistFromArtist(result, fallbackUsed));
        } else if (type == "label") {
            // currently not supported
            continue;
        } else {
            continue;
        }
    }
    return searchResult;
}

std::vector<std::shared_ptr<IAlbum>>
DiscogsAPI::searchRelease(SearchParams _params, std::size_t _perPage,
                          std::size_t _page) {

    _params.type = "master";
    auto results = search(_params, _perPage, _page);
    if (results.albums.empty()) {
        _params.type = "release";
        results = search(_params, _perPage, _page);
    }
    std::cout << "return " << results.albums.size() << " results" << std::endl;
    return results.albums;

    // std::unordered_set<int> masterIds;

    // for (auto &&result : results.albums) {
    //     if (!result) {
    //         std::cerr << "Invalid search result!" << std::endl;
    //         continue;
    //     }

    // int id(std::dynamic_pointer_cast<Release>(result)->get_masterId());

    // if (id != 0) {
    //     masterIds.insert(id);
    // }
    // }

    // if (masterIds.size() == 1) {
    //     return {getMasterRelease(*masterIds.begin())};
    // }

    // std::vector<std::shared_ptr<IAlbum>> verifiedReleases;
    // std::vector<std::shared_ptr<IAlbum>> fallbackReleases;
    // if (masterIds.empty()) {
    //     for (auto &&result : results.albums) {
    //         std::shared_ptr<Discogs::Release> release = nullptr;
    //         int masterId(
    //             std::dynamic_pointer_cast<Release>(result)->get_masterId());
    //         int id = std::stoi(result->get_id());

    //         if (id == masterId) {
    //             release = getMasterRelease(id);
    //         } else {
    //             release = getRelease(id);
    //         }

    //         if (release->isVerified()) {
    //             verifiedReleases.push_back(release);
    //         } else {
    //             fallbackReleases.push_back(release);
    //         }
    //     }

    //     if (!verifiedReleases.empty()) {
    //         return verifiedReleases;
    //     }

    //     return fallbackReleases;
    // }

    // for (auto &&masterId : masterIds) {
    //     verifiedReleases.push_back(getMasterRelease(masterId));
    // }
    // return verifiedReleases;
}

ISearchResult DiscogsAPI::searchReleaseTrack(SearchParams _params,
                                             std::size_t _perPage,
                                             std::size_t _page) {
    _params.categories.insert(ISearchResult::SearchCategory::Album);
    ISearchResult results;
    results.albums = searchRelease(_params, _perPage, _page);

    for (auto &&release : results.albums) {
        release->ensureLoaded();
        results.tracks.insert(results.tracks.end(),
                              release->get_tracklist().begin(),
                              release->get_tracklist().end());
    }
    return results;
}

std::shared_ptr<Release> DiscogsAPI::getRelease(int _releaseId,
                                                const std::string &_currAbbr) {
    if (auto it = cachedReleases.find(_releaseId); it != cachedReleases.end()) {
        return it->second;
    }
    return createRelease(performRequest(
        "/releases/" + std::to_string(_releaseId) + "?" + _currAbbr));
}

std::shared_ptr<Release> DiscogsAPI::getMasterRelease(int _masterId) {
    return createRelease(
        performRequest("/masters/" + std::to_string(_masterId)));
}

bool DiscogsAPI::load(std::shared_ptr<IMediaEntity> _obj) {
    if (auto artist = std::dynamic_pointer_cast<Artist>(_obj))
        return loadAdditionalData(artist);
    // else if (auto label = std::dynamic_pointer_cast<Discogs::Label>(_obj))
    //     loadAdditionalData(label);
    else if (auto release = std::dynamic_pointer_cast<Release>(_obj))
        return loadAdditionalData(release);
    return false;
}

bool DiscogsAPI::loadAdditionalData(std::shared_ptr<Artist> _artist) {
    std::cerr << "Not yet implemented!" << std::endl;
    return false;
}
bool DiscogsAPI::loadAdditionalData(std::shared_ptr<Release> _release) {
    json jsonFullAlbum;
    if (_release->get_masterId() != 0) {
        jsonFullAlbum = performRequest(
            "/masters/" + std::to_string(_release->get_masterId()));
    } else {
        jsonFullAlbum = performRequest("/releases/" + _release->get_id());
    }

    return (insertTracklist(_release, jsonFullAlbum) &&
            insertArtists(_release, jsonFullAlbum));
}
bool DiscogsAPI::loadAdditionalData(std::shared_ptr<Label> _playlist) {
    std::cerr << "Not yet implemented!" << std::endl;
    return false;
}

std::shared_ptr<Artist>
DiscogsAPI::createArtistFromArtist(const json &_jsonArtist,
                                   bool &_fallbackUsed) {
    return std::make_shared<Artist>(
        _jsonArtist.at("id").get<int>(),
        normalizeArtistName(_jsonArtist.at("title").get<std::string>()),
        Artist::State::Full, parseImageUrl(_jsonArtist, _fallbackUsed), this);
}

std::shared_ptr<Artist>
DiscogsAPI::createArtistFromRelease(const json &_jsonArtist) {
    bool ignoreFallbackUsed;
    return std::make_shared<Artist>(
        _jsonArtist.at("id").get<int>(),
        normalizeArtistName(_jsonArtist.at("name").get<std::string>()),
        Artist::State::Preview, parseImageUrl(_jsonArtist, ignoreFallbackUsed),
        this);
}

std::vector<std::shared_ptr<IArtist>>
DiscogsAPI::createArtists(const json &_jsonArtists) {
    std::vector<std::shared_ptr<IArtist>> artists;
    for (auto &&jsonArtist : _jsonArtists) {
        artists.push_back(createArtistFromRelease(jsonArtist));
    }
    return artists;
}

std::vector<Label> DiscogsAPI::createLabels(const json &_jsonLabels,
                                            bool &_fallbackUsed) {
    std::vector<Label> labels;
    for (auto &&jsonLabel : _jsonLabels) {
        labels.emplace_back(jsonLabel.at("id").get<int>(),
                            jsonLabel.at("name").get<std::string>(),
                            Label::State::Partial,
                            parseImageUrl(jsonLabel, _fallbackUsed), this);
    }
    return labels;
}

std::shared_ptr<Release> DiscogsAPI::createRelease(const json &_jsonRelease) {

    const auto id = _jsonRelease.at("id").get<std::size_t>();
    if (auto it = cachedReleases.find(id); it != cachedReleases.end()) {
        return it->second;
    }

    bool fallbackUsed(false);
    std::optional<std::size_t> year;
    if (_jsonRelease.contains("year")) {
        if (_jsonRelease["year"].is_string()) {
            year.emplace(std::stoi(_jsonRelease["year"].get<std::string>()));
        } else {
            year.emplace(_jsonRelease["year"].get<std::size_t>());
        }
    }

    auto release = std::make_shared<Release>(
        id, normalizeReleaseName(_jsonRelease.at("title").get<std::string>()),
        Release::State::Full, parseImageUrl(_jsonRelease, fallbackUsed),
        createArtists(
            getOptional<json>(_jsonRelease, "artists", {}, fallbackUsed)),
        getOptional<int>(_jsonRelease, "master_id", 0, fallbackUsed),
        IAlbum::album_type_t::UNDEFINED,
        parseStyles(_jsonRelease, fallbackUsed),
        createLabels(
            getOptional<json>(_jsonRelease, "labels", {}, fallbackUsed),
            fallbackUsed),
        "<copyright>", year, parseVideos(_jsonRelease, fallbackUsed),
        getOptional<std::string>(_jsonRelease, "data_quality", "",
                                 fallbackUsed) == "Correct",
        this);

    if (fallbackUsed || !insertTracklist(release, _jsonRelease)) {
        release->set_state(Release::State::Partial);
    }

    return cachedReleases.emplace(id, std::move(release)).first->second;
}

bool DiscogsAPI::insertTracklist(std::shared_ptr<Release> _release,
                                 const json &_jsonAlbum) {
    if (!_jsonAlbum.contains("tracklist") || _jsonAlbum["tracklist"].empty()) {
        return false;
    }
    std::vector<std::shared_ptr<ITrack>> tracks;
    bool fallbackUsed(false);
    for (auto &&jsonTrack : _jsonAlbum["tracklist"]) {
        if (!jsonTrack.contains("type_") ||
            jsonTrack["type_"].get<std::string>() != "track") {
            continue;
        }

        std::vector<std::shared_ptr<IArtist>> artists;
        // 'extraArtists' only lists additional artists, whereas artists has all
        // artists including the album artist
        if (jsonTrack.contains("extraartists")) {
            bool ignoreFallback;
            auto extraArtists = createArtists(jsonTrack["extraartists"]);
            artists = _release->get_artists();
            artists.insert(artists.end(), extraArtists.begin(),
                           extraArtists.end());
        } else if (jsonTrack.contains("artists")) {
            artists = createArtists(jsonTrack["artists"]);
        } else {
            artists = _release->get_artists();
        }

        auto position = normalizePosition(
            getOptional<std::string>(jsonTrack, "position", {}, fallbackUsed));

        tracks.push_back(std::make_shared<ReleaseTrack>(
            _release->get_id(),
            getOptional<std::string>(jsonTrack, "title", {}, fallbackUsed),
            _release, artists,
            ReleaseTrack::parseDuration(getOptional<std::string>(
                jsonTrack, "duration", {}, fallbackUsed)),
            position.first, position.second, this));
    }
    _release->set_tracklist(tracks);
    return true;
}

bool DiscogsAPI::insertArtists(std::shared_ptr<Release> _release,
                               const json &_jsonAlbum) {
    if (!_jsonAlbum.contains("artists") || _jsonAlbum["artists"].empty()) {
        return false;
    }

    auto artists = createArtists(_jsonAlbum["artists"]);
    if (artists.empty()) {
        return false;
    }
    _release->set_artists(artists);
    return true;
}

std::vector<std::string> DiscogsAPI::parseGenres(const json &_j,
                                                 bool &_fallbackUsed) {
    std::vector<std::string> genres;
    if (auto it = _j.find("genre"); it != _j.end()) {
        for (auto &&jsonGenre : *it) {
            genres.push_back(jsonGenre.get<std::string>());
        }
        return genres;
    }
    if (auto it = _j.find("genres"); it != _j.end()) {
        for (auto &&jsonGenre : *it) {
            genres.push_back(jsonGenre.get<std::string>());
        }
        return genres;
    }
    _fallbackUsed = true;
    return genres;
}

std::vector<std::string> DiscogsAPI::parseStyles(const json &_j,
                                                 bool &_fallbackUsed) {
    std::vector<std::string> styles;
    if (auto it = _j.find("style"); it != _j.end()) {
        for (auto &&jsonStyle : *it) {
            styles.push_back(jsonStyle.get<std::string>());
        }
        return styles;
    }
    if (auto it = _j.find("styles"); it != _j.end()) {
        for (auto &&jsonStyle : *it) {
            styles.push_back(jsonStyle.get<std::string>());
        }
        return styles;
    }
    _fallbackUsed = true;
    return styles;
}

std::vector<Release::Video> DiscogsAPI::parseVideos(const json &_j,
                                                    bool &_fallbackUsed) {
    std::vector<Release::Video> videos;
    for (auto &&jsonVideo :
         getOptional<json>(_j, "videos", {}, _fallbackUsed)) {
        videos.push_back(
            {getOptional<std::string>(jsonVideo, "description", {},
                                      _fallbackUsed),
             getOptional<std::string>(jsonVideo, "title", {}, _fallbackUsed),
             getOptional<std::string>(jsonVideo, "uri", {}, _fallbackUsed),
             getOptional<int>(jsonVideo, "duration", {}, _fallbackUsed)});
    }
    return videos;
}

std::string DiscogsAPI::parseImageUrl(const json &_j, bool &_fallbackUsed) {
    if (auto it = _j.find("images"); it != _j.end() && !it->empty())
        return (*it)[0].value("resource_url", "");
    if (auto it = _j.find("cover_image"); it != _j.end() && !it->empty())
        return it->get<std::string>();
    if (auto it = _j.find("thumbnail_url"); it != _j.end() && !it->empty())
        return it->get<std::string>();

    _fallbackUsed = true;
    return {};
}

std::string DiscogsAPI::normalizeArtistName(std::string _name) {
    auto close = _name.rfind(')');

    if (close == std::string::npos || close != _name.size() - 1)
        return _name;

    auto open = _name.rfind('(');

    if (open == std::string::npos || open >= close)
        return _name;

    for (std::size_t i = open + 1; i < close; ++i) {
        if (!std::isdigit(static_cast<unsigned char>(_name[i]))) {
            return _name;
        }
    }

    // remove trailing " (123)"
    if (open > 0 && _name[open - 1] == ' ')
        --open;

    _name.erase(open);

    return _name;
}

std::string DiscogsAPI::normalizeReleaseName(std::string _name) {
    auto delimiter = _name.find('-');

    if (delimiter == std::string::npos || delimiter > _name.size() - 2) {
        return _name;
    }

    return _name.substr(delimiter + 2, _name.size() - delimiter - 2);
}

/**
 * @brief
 *
 *
 * @param _position
 * @return discnumber, tracknumber
 */
std::pair<std::optional<std::size_t>, std::optional<std::size_t>>
DiscogsAPI::normalizePosition(std::string _position) {

    std::smatch match;

    auto trim = [](std::string &s) {
        s.erase(0, s.find_first_not_of(" \t"));
        s.erase(s.find_last_not_of(" \t") + 1);
    };

    trim(_position);

    // 1-1 / 1.1 / 1/1
    static const std::regex discTrack(R"(^(\d+)[\-./](\d+)$)");
    if (std::regex_match(_position, match, discTrack)) {
        return {std::stoul(match[1].str()), std::stoul(match[2].str())};
    }

    // A1 / B2 / C3
    static const std::regex vinyl(R"(^([A-Z]+)(\d+)$)");
    if (std::regex_match(_position, match, vinyl)) {
        const std::string side = match[1].str();
        const std::size_t track = std::stoul(match[2].str());

        char c = side[0];
        int disc = ((c - 'A') / 2) + 1;

        return {disc, track};
    }

    // numeric only
    static const std::regex numeric(R"(^(\d+)$)");
    if (std::regex_match(_position, match, numeric)) {
        return {1, std::stoul(match[1].str())};
    }

    return {std::nullopt, std::nullopt};
}

} // namespace Discogs