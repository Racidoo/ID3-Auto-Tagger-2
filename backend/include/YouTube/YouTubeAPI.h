#if !defined(YOUTUBE_H)
#define YOUTUBE_H

#include <curl/curl.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

#include "Query.h"
#include "Services/IMediaService.hpp"
#include "YouTube/Video.h"

class ITrack;

namespace YouTube {

class YouTubeAPI : public Query, public IMediaService {
  public:
    YouTubeAPI();
    YouTubeAPI(const std::string &_accessToken);
    ~YouTubeAPI();

    void saveCredentials() override;
    std::string generateAccessToken() override;

    std::string findBestMatch(std::shared_ptr<ITrack> _track,
                              std::function<void(int)> _onProgress);
    std::shared_ptr<Video> getVideo(const std::string &_id);
    std::vector<std::shared_ptr<Video>>
    searchVideo(const std::string &_query,
                std::string *_nextPageToken = nullptr, unsigned int _limit = 0);

    void load(std::shared_ptr<IMediaEntity> _obj) override;

    void loadAdditionalData(std::shared_ptr<Video> _video);

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

    std::shared_ptr<Video> createVideo(const json &_jsonVideo) const;
    json searchList(const std::string &_query, std::string *_nextPageToken,
                    unsigned int _maxResults = 0,
                    const std::string &_type = "video");
    json fetchContentDetails(const std::string &_id);

    bool findInString(const std::string &title, const std::string &artist);
    unsigned int parse_duration(const std::string &iso8601_duration) const;
};

} // namespace YouTube
#endif // YOUTUBE_H
