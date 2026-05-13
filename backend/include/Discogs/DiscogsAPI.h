#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_set>

using json = nlohmann::json;

#include "Discogs/Release.h"
#include "Discogs/ReleaseTrack.h"
#include "Interfaces/ISearchResult.hpp"
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
        std::unordered_set<ISearchResult::SearchCategory> categories;
    };

    std::vector<std::shared_ptr<Release>>
    searchRelease(const SearchParams &_params);
    ISearchResult search(const SearchParams &_params);

    std::shared_ptr<Release> getMasterRelease(int _masterId);
    std::shared_ptr<Release> getRelease(int _releaseId,
                                        const std::string &_currAbbr = "EUR");
    std::vector<std::shared_ptr<ReleaseTrack>>
    getReleaseTracks(std::shared_ptr<Release> _release);

  protected:
    void prepareHeaders(struct curl_slist *&_headers) override;
    std::string prepareUrl(const std::string &_url) override;

  private:
    std::string accessToken;
    inline static const std::string urlAPI = "https://api.discogs.com";

    Artist createArtistFromRelease(const json &_jsonArtist,
                                   bool &_fallbackUsed);
    Artist createArtistFromArtist(const json &_jsonArtist, bool &_fallbackUsed);
    std::vector<Artist> createArtists(const json &_jsonArtists,
                                      bool &_fallbackUsed);
    std::vector<Label> createLabels(const json &_jsonLabels,
                                    bool &_fallbackUsed);

    std::shared_ptr<Release> createReleaseFromSearch(const json &_jsonRelease);
    std::shared_ptr<Release> createRelease(const json &_jsonRelease);

    static std::vector<std::string> parseGenresFromSearch(const json &_j,
                                                          bool &_fallbackUsed);
    static std::vector<std::string> parseStylesFromSearch(const json &_j,
                                                          bool &_fallbackUsed);
    static std::vector<std::string> parseGenres(const json &_j,
                                                bool &_fallbackUsed);
    static std::vector<std::string> parseStyles(const json &_j,
                                                bool &_fallbackUsed);
    static std::vector<ReleaseTrack> parseTracklist(const json &_j,
                                                    const Release &_release,
                                                    bool &_fallbackUsed);
    static std::vector<Release::Video> parseVideos(const json &_j,
                                                   bool &_fallbackUsed);
    static std::string parseImageUrl(const json &_j, bool &_fallbackUsed);
};

} // namespace Discogs