#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_set>

using json = nlohmann::json;

#include "Discogs/MasterRelease.h"
#include "Discogs/Release.h"
#include "Discogs/SearchResult.h"
#include "Query.h"

namespace Discogs {

class DiscogsAPI : public Query {
  public:
    DiscogsAPI() = default;
    DiscogsAPI(const std::string &_accessToken);

    void saveCredentials() override;
    std::string generateAccessToken() override;

    struct SearchParams {
        std::optional<std::string> query;
        std::optional<std::string> type;
        std::optional<std::string> title;
        std::optional<std::string> releaseTitle;
        std::optional<std::string> credit;
        std::optional<std::string> artist;
        std::optional<std::string> anv;
        std::optional<std::string> label;
        std::optional<std::string> genre;
        std::optional<std::string> style;
        std::optional<std::string> country;
        std::optional<int> year;
        std::optional<std::string> format;
        std::optional<std::string> catno;
        std::optional<std::string> barcode;
        std::optional<std::string> track;
        std::optional<std::string> submitter;
        std::optional<std::string> contributor;
    };

    std::vector<Release> searchRelease(const SearchParams &_params);
    std::vector<MasterRelease> searchMasterRelease(const SearchParams &_params);

    MasterRelease getMasterRelease(int _masterId);
    Release getRelease(int _releaseId, const std::string &_currAbbr = "EUR");

    std::vector<SearchResult> search(const SearchParams &_params);

  protected:
    void prepareHeaders(struct curl_slist *&_headers) override;
    std::string prepareUrl(const std::string &_url) override;

  private:
    std::string accessToken;
    inline static const std::string urlAPI = "https://api.discogs.com";

    std::vector<Artist> createArtists(const json &_jsonArtists);
    std::vector<Label> createLabels(const json &_jsonLabels);
    MasterRelease createMasterRelease(const json &_jsonRelease);
    Release createRelease(const json &_jsonRelease);

    static std::vector<std::string> parseGenres(const json &_j);
    static std::vector<std::string> parseStyles(const json &_j);
    static std::vector<Release::Track> parseTracklist(const json &_j);
    static std::vector<Release::Video> parseVideos(const json &_j);
    static std::string parseImageUrl(const json &_j);
};

} // namespace Discogs