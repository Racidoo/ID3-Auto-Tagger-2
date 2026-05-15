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
            searchResult.albums.push_back(
                IAlbum::fromDiscogs(createReleaseFromSearch(result)));
        } else if (type == "artist" &&
                   _params.categories.find(
                       ISearchResult::SearchCategory::Artist) !=
                       _params.categories.end()) {
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
    }
    return searchResult;
}

std::vector<std::shared_ptr<Release>>
DiscogsAPI::searchRelease(const SearchParams &_params) {

    auto results = search(_params);

    if (results.albums.empty())
        return {};

    std::unordered_set<int> masterIds;

    for (auto &&result : results.albums) {
        int id(0);
        if (result->get_discogsRelease()) {
            id = result->get_discogsRelease()->get_masterId();
        }
        if (id != 0) {
            masterIds.insert(id);
        }
    }

    if (masterIds.size() == 1) {
        return {getMasterRelease(*masterIds.begin())};
    }

    std::vector<std::shared_ptr<Release>> verifiedReleases;
    std::vector<std::shared_ptr<Release>> fallbackReleases;
    if (masterIds.empty()) {
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

    for (auto &&masterId : masterIds) {
        verifiedReleases.push_back(getMasterRelease(masterId));
    }
    return verifiedReleases;
}

std::shared_ptr<Release> DiscogsAPI::getRelease(int _releaseId,
                                                const std::string &_currAbbr) {
    return createRelease(performRequest(
        "/releases/" + std::to_string(_releaseId) + "?" + _currAbbr));
}

std::shared_ptr<Release> DiscogsAPI::getMasterRelease(int _masterId) {
    return createRelease(
        performRequest("/masters/" + std::to_string(_masterId)));
}

// std::vector<::shared_ptr<ReleaseTrack>>
// DiscogsAPI::getReleaseTracks(std::shared_ptr<Release> _release) {
//     std::vector<std::shared_ptr<ReleaseTrack>> releaseTracks;
//     std::shared_ptr<Release> newRelease = nullptr;
//     if (_release->get_tracklist().empty()) {
//         int id = std::stoi(_release->get_id());
//         if (id == _release->get_masterId()) {
//             newRelease = getMasterRelease(_release->get_masterId());
//         } else {
//             newRelease = getRelease(id);
//         }
//     } else {
//         newRelease = _release;
//     }

//     // for (auto &&track : newRelease->get_tracklist()) {
//     //     releaseTracks.push_back(std::make_shared<ReleaseTrack>(
//     //         newRelease->get_id(), track.title, *newRelease,
//     //         newRelease->get_artists(), track.extraArtists,
//     //         newRelease->get_genres(), newRelease->get_imageUrl(),
//     //         newRelease->get_labels(),
//     //         ReleaseTrack::parseDuration(track.duration), track.position,
//     //         newRelease->get_year()));
//     // }
//     return releaseTracks;
// }

void DiscogsAPI::load(IMediaEntity &_obj) {
    if (auto *artist = dynamic_cast<Discogs::Artist *>(&_obj))
        loadAdditionalData(*artist);
    else if (auto *label = dynamic_cast<Discogs::Label *>(&_obj))
        loadAdditionalData(*label);
    else if (auto *release = dynamic_cast<Discogs::Release *>(&_obj))
        loadAdditionalData(*release);
}

void DiscogsAPI::loadAdditionalData(Artist &_artist) {}
void DiscogsAPI::loadAdditionalData(Release &_album) {}
void DiscogsAPI::loadAdditionalData(Label &_playlist) {}

Artist DiscogsAPI::createArtistFromArtist(const json &_jsonArtist,
                                          bool &_fallbackUsed) {
    return Artist(_jsonArtist.at("id").get<int>(),
                  _jsonArtist.at("title").get<std::string>(),
                  Artist::State::Full,
                  getOptional<std::string>(_jsonArtist, "cover_image", {},
                                           _fallbackUsed));
}

Artist DiscogsAPI::createArtistFromRelease(const json &_jsonArtist,
                                           bool &_fallbackUsed) {
    return Artist(_jsonArtist.at("id").get<int>(),
                  _jsonArtist.at("name").get<std::string>(),
                  Artist::State::Preview,
                  getOptional<std::string>(_jsonArtist, "thumbnail_url", {},
                                           _fallbackUsed));
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
                            Label::State::Partial,
                            jsonLabel.at("thumbnail_url").get<std::string>());
    }
    return labels;
}

std::shared_ptr<Release>
DiscogsAPI::createReleaseFromSearch(const json &_jsonRelease) {
    bool fallbackUsed(false);
    Release release(
        _jsonRelease.at("id").get<int>(),
        _jsonRelease.at("title").get<std::string>(), Release::State::Full,
        getOptional<std::string>(_jsonRelease, "cover_image", {}, fallbackUsed),
        createArtists(
            getOptional<json>(_jsonRelease, "artists", {}, fallbackUsed),
            fallbackUsed),
        getOptional<int>(_jsonRelease, "master_id", 0, fallbackUsed),
        getOptional<std::string>(_jsonRelease, "country", "", fallbackUsed),
        parseGenresFromSearch(_jsonRelease, fallbackUsed),
        createLabels(
            getOptional<json>(_jsonRelease, "labels", {}, fallbackUsed),
            fallbackUsed),
        getOptional<std::string>(_jsonRelease, "released", "", fallbackUsed),
        parseStylesFromSearch(_jsonRelease, fallbackUsed),
        std::stoi(
            getOptional<std::string>(_jsonRelease, "year", "0", fallbackUsed)),
        parseVideos(_jsonRelease, fallbackUsed),
        getOptional<std::string>(_jsonRelease, "data_quality", "",
                                 fallbackUsed) == "Correct");
    release.set_tracklist(parseTracklist(_jsonRelease, release, fallbackUsed));
    if (fallbackUsed) {
        release.set_state(Release::State::Partial);
    }
    return std::make_shared<Release>(release);
}

std::shared_ptr<Release> DiscogsAPI::createRelease(const json &_jsonRelease) {
    bool fallbackUsed(false);
    Release release(
        _jsonRelease.at("id").get<int>(),
        _jsonRelease.at("title").get<std::string>(), Release::State::Full,
        getOptional<std::string>(_jsonRelease, "cover_image", {}, fallbackUsed),
        createArtists(
            getOptional<json>(_jsonRelease, "artists", {}, fallbackUsed),
            fallbackUsed),
        getOptional<int>(_jsonRelease, "master_id", 0, fallbackUsed),
        getOptional<std::string>(_jsonRelease, "country", "", fallbackUsed),
        parseGenres(_jsonRelease, fallbackUsed),
        createLabels(
            getOptional<json>(_jsonRelease, "labels", {}, fallbackUsed),
            fallbackUsed),
        getOptional<std::string>(_jsonRelease, "released", "", fallbackUsed),
        parseStyles(_jsonRelease, fallbackUsed),
        getOptional<int>(_jsonRelease, "year", 0, fallbackUsed),
        parseVideos(_jsonRelease, fallbackUsed),
        getOptional<std::string>(_jsonRelease, "data_quality", "",
                                 fallbackUsed) == "Correct");
    release.set_tracklist(parseTracklist(_jsonRelease, release, fallbackUsed));
    if (fallbackUsed) {
        release.set_state(Release::State::Partial);
    }
    return std::make_shared<Release>(release);
}

std::vector<std::string>
DiscogsAPI::parseGenresFromSearch(const json &_j, bool &_fallbackUsed) {
    std::vector<std::string> genres;
    for (auto &&jsonGenre : getOptional<json>(_j, "genre", {}, _fallbackUsed)) {
        genres.push_back(jsonGenre.get<std::string>());
    }
    return genres;
}

std::vector<std::string>
DiscogsAPI::parseStylesFromSearch(const json &_j, bool &_fallbackUsed) {
    std::vector<std::string> styles;
    for (auto &&jsonStyle : getOptional<json>(_j, "style", {}, _fallbackUsed)) {
        styles.push_back(jsonStyle.get<std::string>());
    }
    return styles;
}

std::vector<std::string> DiscogsAPI::parseGenres(const json &_j,
                                                 bool &_fallbackUsed) {
    std::vector<std::string> genres;
    for (auto &&jsonGenre :
         getOptional<json>(_j, "genres", {}, _fallbackUsed)) {
        genres.push_back(jsonGenre.get<std::string>());
    }
    return genres;
}

std::vector<std::string> DiscogsAPI::parseStyles(const json &_j,
                                                 bool &_fallbackUsed) {
    std::vector<std::string> styles;
    for (auto &&jsonStyle :
         getOptional<json>(_j, "styles", {}, _fallbackUsed)) {
        styles.push_back(jsonStyle.get<std::string>());
    }
    return styles;
}

std::vector<ReleaseTrack> DiscogsAPI::parseTracklist(const json &_j,
                                                     const Release &_release,
                                                     bool &_fallbackUsed) {
    std::vector<ReleaseTrack> tracklist;
    for (auto &&jsonTrack :
         getOptional<json>(_j, "tracklist", {}, _fallbackUsed)) {
        json extraArtists =
            getOptional<json>(jsonTrack, "extraartists", {}, _fallbackUsed);

        tracklist.emplace_back(
            _release.get_id(),
            getOptional<std::string>(jsonTrack, "title", {}, _fallbackUsed),
            _release, _release.get_artists(), std::vector<Artist>{},
            // createArtists(extraArtists, _fallbackUsed),
            _release.get_genres(), _release.get_imageUrl(),
            _release.get_labels(),
            ReleaseTrack::parseDuration(getOptional<std::string>(
                jsonTrack, "duration", {}, _fallbackUsed)),
            getOptional<std::string>(jsonTrack, "position", {}, _fallbackUsed),
            _release.get_year());
    }
    return tracklist;
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
    _fallbackUsed = true;
    return {};
}

} // namespace Discogs