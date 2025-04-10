#include "../include/YouTube.h"
#include "../include/Spotify/SpotifyAPI.h"

YouTube::YouTube() : Query("Google") {}

YouTube::~YouTube() {}

json YouTube::query(const std::string &_url) {
    std::string response;
    auto queryCurl(curl_easy_init());
    curl_easy_setopt(queryCurl, CURLOPT_URL, _url.c_str());
    curl_easy_setopt(queryCurl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(queryCurl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(queryCurl);
    if (res != CURLE_OK) {
        std::cerr << "Curl request failed: " << curl_easy_strerror(res)
                  << std::endl;
    }
    if (queryCurl)
        curl_easy_cleanup(queryCurl);
    return json::parse(response);
}

// Function to perform YouTube API search
json YouTube::searchList(const std::string &_query, const std::string &_service,
                         unsigned int _maxResults, const std::string &_type) {
    if (_service == "yt-dlp") {
        auto result = exec(std::string("yt-dlp \"ytsearch") +
                           std::to_string(_maxResults) + ':' + _query +
                           "\" --print-json --simulate --skip-download");
        std::ofstream("yt-dlp_result.json") << result.dump(4);
        return result;
    } else {
        // simulate googleAPI failure
        // json result = json::parse(
        //     "{\"error\":{\"code\":403,\"message\":"
        //     "\"Therequestcannotbecompletedbecauseyouhavereachedyourdailylimitfo"
        //     "rquota.\",\"errors\":[{\"message\":"
        //     "\"Therequestcannotbecompletedbecauseyouhavereachedyourdailylimitfo"
        //     "rquota.\",\"domain\":\"usageLimits\",\"reason\":"
        //     "\"dailyLimitExceeded\"}],\"status\":\"FAILED\"}}");

        auto result =
            query(searchListUrl + std::string(curl_escape(_query.c_str(), 0)) +
                  "&type=" + _type.c_str() + "&maxResults=" +
                  std::to_string(_maxResults) + "&key=" + accessToken);
        std::ofstream("result.json") << result.dump(4);
        return result;
    }
}

json YouTube::searchContentDetails(const std::string &_videoID) {
    return query(searchContentDetailsUrl + _videoID + "&key=" + accessToken);
}

// Check if artist name appears in video title
bool YouTube::findInString(const std::string &title,
                           const std::string &artist) {
    return toLower(title).find(toLower(artist)) != std::string::npos;
}

// Function to extract the video duration from the YouTube API response
unsigned int YouTube::parse_duration(const std::string &iso8601_duration) {
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

// Select best match from YouTube results
std::string YouTube::findBestMatch(const Spotify::Track &_track) {
    double bestScore = 0.0;
    std::vector<std::string> topMatches;
    std::string searchQuery =
        _track.get_stringArtists() + " - " + _track.get_name();
    std::cout << "Search query: " << searchQuery << std::endl;

    bool googleAPI(true);

    // ToDo: API limit reached, fallback to yt-dlp
    auto responseSearchList = searchList(searchQuery, "google");
    if (responseSearchList.contains("error")) {
        if (responseSearchList["error"].contains("code") &&
            responseSearchList["error"]["code"] == 403) {
            // If Quota is reached try slower, fallback api
            responseSearchList = searchList(searchQuery, "yt-dlp");
            googleAPI = false;
        } else {
            std::cerr << "Unknown error!:\n"
                      << responseSearchList.dump(4) << std::endl;
            return "";
        }
    } else {
        responseSearchList = responseSearchList["items"];
    }
    // std::cout << responseSearchList.dump(4) << std::endl;
    for (const auto &video : responseSearchList) {
        // skip all non-videos
        if (googleAPI) {
            if (video["id"]["kind"] != "youtube#video") {
                continue;
            }
        } else {
            if (video["_type"] != "video") {
                continue;
            }
        }

        std::string title =
            googleAPI ? video["snippet"]["title"] : video["title"];
        std::string channel =
            googleAPI ? video["snippet"]["channelTitle"] : video["channel"];
        std::string id = googleAPI ? video["id"]["videoId"] : video["id"];
        std::string uploadDate =
            googleAPI
                ? video["snippet"]["publishedAt"].get<std::string>().substr(0,
                                                                            10)
                : video["upload_date"].get<std::string>();
        unsigned int duration;

        json contentDetails;
        double score(0);
        if (googleAPI) {
            contentDetails = searchContentDetails(id)["items"][0];
            // std::cout << contentDetails.dump(4) << std::endl;
            duration =
                parse_duration(contentDetails["contentDetails"]["duration"]);
        } else {
            duration = responseSearchList["duration"].get<unsigned int>();
        }
        score += similarityScore(_track.get_durationMs() / 1000, duration);
        if (score < 0.1) {
            std::cout << "video length " << duration
                      << " exceeds Spotify track "
                      << _track.get_durationMs() / 1000 << std::endl;
            continue;
        }
        if (findInString(title, _track.get_name())) {
            score += 1;
        }
        if (googleAPI &&
            contentDetails["contentDetails"]["licensedContent"].get<bool>() ==
                true)
            score = 1.0; // Boost if content is licensed

        //  = similarityScore(searchQuery, title);
        // std::cout << score << std::endl;

        for (auto &&artist : _track.get_artists()) {
            if (findInString(title, artist.get_name())) {
                // normalize, so all artists need to be in title for
                // perfect score
                score += 1.0 / _track.get_artists().size();
                std::cout << "Artist: " << score << std::endl;
            }
            if (findInString(channel, artist.get_name())) {
                score += 1.0 / _track.get_artists().size();
                std::cout << "Channel: " << score << std::endl;
            }
        }

        score += similarityScoreDate(_track.get_album().get_releaseDate(),
                                     uploadDate);

        std::cout << "score: " << score << "\t" << id << "\t" << title
                  << std::endl;
        if (score > bestScore) {
            bestScore = score;
            topMatches.clear();
            topMatches.push_back(id);
        } else if (score == bestScore) {
            topMatches.push_back(id);
        }
    }
    if (topMatches.empty()) {
        std::cout << "No matches found!" << std::endl;
        return "";
    }
    return topMatches[0];
}