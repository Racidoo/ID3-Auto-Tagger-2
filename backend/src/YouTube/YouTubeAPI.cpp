#include "../include/YouTube/YouTubeAPI.h"
#include "../include/Spotify/SpotifyAPI.h"

namespace YouTube {

YouTubeAPI::YouTubeAPI() : Query("Google") {}

YouTubeAPI::YouTubeAPI(const std::string &_accessToken)
    : Query("Google", "", "", _accessToken) {}

YouTubeAPI::~YouTubeAPI() {}

json YouTubeAPI::query(const std::string &_url) {
    std::string response;
    auto queryCurl(curl_easy_init());
    curl_easy_setopt(queryCurl, CURLOPT_URL, _url.c_str());
    curl_easy_setopt(queryCurl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(queryCurl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(queryCurl);
    if (res != CURLE_OK) {
        std::cerr << "Curl request failed: " << curl_easy_strerror(res)
                  << ". URL: " << _url << std::endl;
    }
    if (queryCurl)
        curl_easy_cleanup(queryCurl);
    std::ofstream("query.json") << json::parse(response).dump(4);
    return json::parse(response);
}

// Function to perform YouTubeAPI search
json YouTubeAPI::searchList(const std::string &_query,
                            std::string *_nextPageToken,
                            unsigned int _maxResults,
                            const std::string &_type) {
    // simulate googleAPI failure
    // json result = json::parse(
    //     "{\"error\":{\"code\":403,\"message\":"
    //     "\"Therequestcannotbecompletedbecauseyouhavereachedyourdailylimitfo"
    //     "rquota.\",\"errors\":[{\"message\":"
    //     "\"Therequestcannotbecompletedbecauseyouhavereachedyourdailylimitfo"
    //     "rquota.\",\"domain\":\"usageLimits\",\"reason\":"
    //     "\"dailyLimitExceeded\"}],\"status\":\"FAILED\"}}");

    std::string url =
        urlAPI +
        "search?part=snippet&q=" + std::string(curl_escape(_query.c_str(), 0)) +
        "&type=" + _type +
        (_maxResults != 0 ? "&maxResults=" + std::to_string(_maxResults) : "") +
        "&key=" + getValidToken();

    if (_nextPageToken && !_nextPageToken->empty())
        url += "&pageToken=" + *_nextPageToken;

    json result = query(url);

    if (result.contains("error")) {
        if (result["error"].contains("code")) {

            std::cerr << "Unknown error!:\n" << result.dump(4) << std::endl;
            std::cerr << "Trying fallback 'yt-dlp'" << std::endl;

            result = exec(std::string("yt-dlp \"ytsearch") +
                          std::to_string(_maxResults) + ':' + _query +
                          "\" --print-json --simulate --skip-download");
            result["searchEngine"] = "yt-dlp";
            std::ofstream("yt-dlp_result.json") << result.dump(4);
            return result;
        }
    }

    if (_nextPageToken && result.contains("nextPageToken"))
        *_nextPageToken = result["nextPageToken"].get<std::string>();
    std::ofstream("searchList.json") << result.dump(4);
    return result;
}

json YouTubeAPI::fetchContentDetails(const std::string &_id) {
    json response = query(urlAPI + "videos?part=contentDetails&id=" + _id +
                          "&fields=items(id,kind,contentDetails(duration,"
                          "licensedContent),statistics(viewCount))&key=" +
                          getValidToken());
    if (!response.contains("items") || response["items"].empty())
        throw std::runtime_error("No contentDetails found for video ID: " +
                                 _id);
    return response["items"].front()["contentDetails"];
}

std::unique_ptr<Video> YouTubeAPI::createVideo(const json &_jsonVideo) const {
    auto safe_get = [](const json &j, const std::string &key) -> const json * {
        if (j.contains(key)) {
            return &j.at(key);
        } else {
            std::cerr << "[DEBUG] Missing required key: " << key << "\n";
            return nullptr;
        }
    };

    const bool googleAPI = !_jsonVideo.contains("_type");

    std::string type;

    if (googleAPI) {
        if (const json *kind = safe_get(_jsonVideo, "kind"))
            type = kind->get<std::string>();
    } else {
        if (const json *_type = safe_get(_jsonVideo, "_type"))
            type = _type->get<std::string>();
    }

    if (type.empty() || type.find("video") == std::string::npos) {
        std::cout << "[DEBUG] Skipped non-video entry\n";
        return nullptr;
    }

    // Extract video ID
    std::string id;
    if (googleAPI) {
        if (const json *rawId = safe_get(_jsonVideo, "id"))
            id = rawId->get<std::string>();
    } else {
        if (const json *rawId = safe_get(_jsonVideo, "id"))
            id = rawId->get<std::string>();
    }

    // Extract snippet fields
    std::string title, channel, uploadDate, thumbnail;
    if (googleAPI) {
        if (const json *snippet = safe_get(_jsonVideo, "snippet")) {
            if (const json *t = safe_get(*snippet, "title"))
                title = t->get<std::string>();
            if (const json *ch = safe_get(*snippet, "channelTitle"))
                channel = ch->get<std::string>();
            if (const json *pub = safe_get(*snippet, "publishedAt"))
                uploadDate = pub->get<std::string>().substr(0, 10);

            if (const json *thumbs = safe_get(*snippet, "thumbnails")) {
                if (const json *high = safe_get(*thumbs, "high")) {
                    if (const json *url = safe_get(*high, "url"))
                        thumbnail = url->get<std::string>();
                }
            }
        }
    } else {
        if (const json *t = safe_get(_jsonVideo, "title"))
            title = t->get<std::string>();
        if (const json *ch = safe_get(_jsonVideo, "channel"))
            channel = ch->get<std::string>();
        if (const json *pub = safe_get(_jsonVideo, "upload_date"))
            uploadDate = pub->get<std::string>();
        if (const json *thumb = safe_get(_jsonVideo, "thumbnails"))
            thumbnail = thumb->get<std::string>();
    }

    // Extract duration and license
    unsigned int duration = 0;
    bool licensed = false;
    if (googleAPI) {
        if (const json *content = safe_get(_jsonVideo, "contentDetails")) {
            if (const json *d = safe_get(*content, "duration"))
                duration = parse_duration(d->get<std::string>());
            if (const json *l = safe_get(*content, "licensedContent"))
                licensed = l->get<bool>();
        }
    } else {
        if (const json *d = safe_get(_jsonVideo, "duration"))
            duration = d->get<unsigned int>();
    }

    return std::make_unique<Video>(id, title, channel, uploadDate, thumbnail,
                                   duration, licensed);
}

std::unique_ptr<Video> YouTubeAPI::getVideo(const std::string &_id) {
    return createVideo(
        query(urlAPI +
              "videos?part=snippet,contentDetails,statistics&id=" + _id +
              "&fields=items(id,kind,snippet(title,channelTitle,publishedAt,"
              "thumbnails/high/url),contentDetails(duration,licensedContent),"
              "statistics(viewCount))&key=" +
              getValidToken())
            .at("items")
            .front());
}

std::vector<std::unique_ptr<Video>>
YouTubeAPI::searchVideo(const std::string &_query, std::string *_nextPageToken,
                        unsigned int _limit) {

    std::vector<std::unique_ptr<Video>> results;
    json list = searchList(_query, _nextPageToken, _limit, "video");

    for (auto &item : list.at("items")) {
        std::string id = item["id"]["videoId"];
        std::string kind = item["id"]["kind"];

        // Fetch contentDetails
        try {
            json contentDetails = fetchContentDetails(id);
            item["contentDetails"] = contentDetails;
            item["id"] = id;     // flatten id object
            item["kind"] = kind; // flatten kind object
        } catch (const std::exception &e) {
            std::cerr << "[ERROR] Skipping video ID " << id << ": " << e.what()
                      << "\n";
            continue;
        }

        auto video = createVideo(item);
        if (video)
            results.push_back(std::move(video));
    }

    return results;
}

// Check if artist name appears in video title
bool YouTubeAPI::findInString(const std::string &title,
                              const std::string &artist) {
    return toLower(title).find(toLower(artist)) != std::string::npos;
}

// Function to extract the video duration from the YouTubeAPI API response
unsigned int
YouTubeAPI::parse_duration(const std::string &iso8601_duration) const {
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    size_t start = iso8601_duration.find("PT") + 1;

    // Check for hours
    size_t hour_pos = iso8601_duration.find("H");
    if (hour_pos != std::string::npos) {
        hours = std::stoi(iso8601_duration.substr(start + 1, hour_pos - start));
    } else
        hour_pos = start;
    // Check for minutes
    size_t min_pos = iso8601_duration.find("M");
    if (min_pos != std::string::npos) {
        minutes = std::stoi(
            iso8601_duration.substr(hour_pos + 1, min_pos - 1 - hour_pos));
    } else
        min_pos = hour_pos;

    // Check for seconds
    size_t sec_pos = iso8601_duration.find("S");
    if (sec_pos != std::string::npos) {
        seconds = std::stoi(
            iso8601_duration.substr(min_pos + 1, sec_pos - min_pos - 1));
    }
    return (hours * 60 + minutes * 60 + seconds); // Total duration in seconds
}

// Select best match from YouTubeAPI results
std::string YouTubeAPI::findBestMatch(const Spotify::Track &_track,
                                      std::function<void(int)> _onProgress) {
    double bestScore = 0.0;
    std::vector<std::string> topMatches;
    std::string searchQuery =
        _track.get_stringArtists() + " - " + _track.get_name();
    std::cout << "Search query: " << searchQuery << std::endl;

    bool googleAPI(true);
    std::string nextPageToken;
    int maxPages = 3; // limit how many times we page through results

    for (int page = 0; page < maxPages; ++page) {
        _onProgress(1);
        std::vector<std::unique_ptr<Video>> videos =
            searchVideo(searchQuery, &nextPageToken, 5);

        _onProgress(5);
        for (const auto &video : videos) {
            double score(0);
            if (!video) {
                continue;
            }

            score += similarityScore(_track.get_durationMs() / 1000,
                                     video->get_duration());
            if (score < 0.1) {
                std::cout << "[" << video->get_id() << "]: video length "
                          << video->get_duration() << " exceeds Spotify track "
                          << _track.get_durationMs() / 1000 << std::endl;
                continue;
            }

            score += similarityScore(video->get_name(), _track.get_name());

            if (googleAPI && video->get_licensed())
                score = 1.0;

            for (auto &&artist : _track.get_artists()) {
                if (findInString(video->get_name(), artist.get_name()))
                    score += 1.0 / _track.get_artists().size();
                if (findInString(video->get_channelTitle(), artist.get_name()))
                    score += 1.0 / _track.get_artists().size();
            }

            score += similarityScoreDate(_track.get_album().get_releaseDate(),
                                         video->get_uploadDate());

            std::cout << "score: " << score << "\t" << video->get_id() << "\t"
                      << video->get_name() << std::endl;
            if (score > bestScore) {
                bestScore = score;
                topMatches.clear();
                topMatches.push_back(video->get_id());
            } else if (score == bestScore) {
                topMatches.push_back(video->get_id());
            }
            _onProgress(5 / videos.size());
        }

        if (!topMatches.empty())
            break; // Stop if we found something good
    }

    if (topMatches.empty()) {
        std::cerr << "No matches found!" << std::endl;
        return "";
    }
    return topMatches[0];
}

} // namespace YouTube