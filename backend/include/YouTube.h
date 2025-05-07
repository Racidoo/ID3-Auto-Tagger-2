#if !defined(YOUTUBE_H)
#define YOUTUBE_H

#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "Query.h"
#include "Spotify/Track.h"

using json = nlohmann::json;

class YouTube : public Query {
  private:
    inline static const std::string searchListUrl =
        "https://www.googleapis.com/youtube/v3/search?part=snippet&q=";
    inline static const std::string searchContentDetailsUrl =
        "https://www.googleapis.com/youtube/v3/videos?part=contentDetails&id=";

  public:
    YouTube();
    YouTube(const std::string &_accessToken);
    ~YouTube();

    json searchList(const std::string &_query,
                    const std::string &_service = "yt-dlp",
                    unsigned int _maxResults = 5,
                    const std::string &_type = "video",
                    const std::string &_nextPageToken = "");
    json searchContentDetails(const std::string &_videoIDs);
    std::string findBestMatch(const Spotify::Track &_track,
                              std::function<void(int)> _onProgress);

  private:
    json query(const std::string &_url);

    bool findInString(const std::string &title, const std::string &artist);
    unsigned int parse_duration(const std::string &iso8601_duration);
};

#endif // YOUTUBE_H
