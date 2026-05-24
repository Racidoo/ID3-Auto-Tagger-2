#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <regex>
#include <string>
#include <unordered_set>

using json = nlohmann::json;

#include "Discogs/Release.h"
#include "Discogs/ReleaseTrack.h"
#include "Interfaces/ISearchResult.hpp"
#include "Query.h"
#include "Services/IMediaService.hpp"

namespace Discogs {

class Artist;
class Label;
class Release;
class ReleaseTracks;

class DiscogsAPI : public Query, public IMediaService {
  public:
    DiscogsAPI() = default;
    DiscogsAPI(const std::string &_accessToken);

    void saveCredentials() override;
    std::string generateAccessToken() override;
    inline constexpr IMediaService::MediaSourceId get_id() const override {
        return MediaSourceId::Discogs;
    }

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

    ISearchResult search(const SearchParams &_params, std::size_t _perPage = 0,
                         std::size_t _page = 0);
    std::vector<std::shared_ptr<IAlbum>> searchRelease(SearchParams _params,
                                                       std::size_t _perPage = 0,
                                                       std::size_t _page = 0);
    ISearchResult searchReleaseTrack(SearchParams _params,
                                     std::size_t _perPage = 0,
                                     std::size_t _page = 0);

    std::shared_ptr<Release> getMasterRelease(int _masterId);
    std::shared_ptr<Release> getRelease(int _releaseId,
                                        const std::string &_currAbbr = "EUR");
    std::vector<std::shared_ptr<ReleaseTrack>>
    getReleaseTracks(std::shared_ptr<Release> _release);

    bool load(std::shared_ptr<IMediaEntity> _obj) override;

    bool loadAdditionalData(std::shared_ptr<Artist> _artist);
    bool loadAdditionalData(std::shared_ptr<Release> _release);
    bool loadAdditionalData(std::shared_ptr<Label> _playlist);

  protected:
    void prepareHeaders(struct curl_slist *&_headers) override;
    std::string prepareUrl(const std::string &_url) override;

  private:
    std::string accessToken;
    inline static const std::string urlAPI = "https://api.discogs.com";

    std::shared_ptr<Artist> createArtistFromRelease(const json &_jsonArtist);
    std::shared_ptr<Artist> createArtistFromArtist(const json &_jsonArtist,
                                                   bool &_fallbackUsed);
    std::vector<std::shared_ptr<IArtist>>
    createArtists(const json &_jsonArtists);
    std::vector<Label> createLabels(const json &_jsonLabels,
                                    bool &_fallbackUsed);

    // std::shared_ptr<Release> createReleaseFromSearch(const json
    // &_jsonRelease);
    std::shared_ptr<Release> createRelease(const json &_jsonRelease);

    bool insertTracklist(std::shared_ptr<Release> _release,
                         const json &_jsonAlbum);
    bool insertArtists(std::shared_ptr<Release> _release,
                       const json &_jsonAlbum);

    static std::vector<std::string> parseGenres(const json &_j,
                                                bool &_fallbackUsed);
    static std::vector<std::string> parseStyles(const json &_j,
                                                bool &_fallbackUsed);

    static std::vector<Release::Video> parseVideos(const json &_j,
                                                   bool &_fallbackUsed);
    static std::string parseImageUrl(const json &_j, bool &_fallbackUsed);

    static std::string normalizeArtistName(std::string _name);
    static std::string normalizeReleaseName(std::string _name);
    static std::pair<std::optional<std::size_t>, std::optional<std::size_t>>
    normalizePosition(std::string _position);
};

} // namespace Discogs