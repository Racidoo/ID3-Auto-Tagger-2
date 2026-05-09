#if !defined(YOUTUBE_H)
#define YOUTUBE_H

#include <curl/curl.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "Query.h"
#include "Spotify/Track.h"
#include "YouTube/Video.h"

namespace YouTube {

using json = nlohmann::json;

class YouTubeAPI : public Query {
  public:
    YouTubeAPI();
    YouTubeAPI(const std::string &_accessToken);
    ~YouTubeAPI();

    void saveCredentials() override;
    std::string generateAccessToken() override;

    std::string findBestMatch(const Spotify::Track &_track,
                              std::function<void(int)> _onProgress);
    std::unique_ptr<Video> getVideo(const std::string &_id);
    std::vector<std::unique_ptr<Video>>
    searchVideo(const std::string &_query,
                std::string *_nextPageToken = nullptr, unsigned int _limit = 0);

  private:
    std::string accessToken;
    inline const static std::string urlAPI =
        "https://www.googleapis.com/youtube/v3";

    // inline static const std::string searchListUrl =
    //     "https://www.googleapis.com/youtube/v3/search?part=snippet&q=";
    // inline static const std::string searchContentDetailsUrl =
    //     "https://www.googleapis.com/youtube/v3/videos?part=contentDetails&id=";

    void prepareHeaders(struct curl_slist *&_headers) override;
    std::string prepareUrl(const std::string &_url) override;

    std::unique_ptr<Video> createVideo(const json &_jsonVideo) const;
    json searchList(const std::string &_query, std::string *_nextPageToken,
                    unsigned int _maxResults = 0,
                    const std::string &_type = "video");
    json fetchContentDetails(const std::string &_id);

    bool findInString(const std::string &title, const std::string &artist);
    unsigned int parse_duration(const std::string &iso8601_duration) const;
};

} // namespace YouTube
#endif // YOUTUBE_H
