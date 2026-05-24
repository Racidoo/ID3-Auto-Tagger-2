#pragma once

#include <memory>
#include <regex>

#include "Discogs/DiscogsAPI.h"
#include "Services/Sources/IMediaSource.hpp"

class DiscogsMediaSource : public IMediaSource {
  public:
    DiscogsMediaSource(Discogs::DiscogsAPI &_discogs);
    ~DiscogsMediaSource() = default;

    IMediaService::MediaSourceId get_id() const override;

    ISearchResult search(const std::string &_query,
                         const SearchOptions &_options) override;
    ISearchResult search(std::shared_ptr<ITrack> _track,
                         const SearchOptions &_options) override;
    bool supports(const std::string &_input) const override;
    ISearchResult resolve(const std::string &_input) override;

    std::shared_ptr<IAlbum> getAlbum(const std::string &_id) override;
    std::shared_ptr<IArtist> getArtist(const std::string &_id) override;
    std::shared_ptr<ITrack> getTrack(const std::string &_id) override;

  private:
    Discogs::DiscogsAPI &discogs;
    std::regex pattern = std::regex(
        R"(https:\/\/www\.discogs\.com\/([label|release|artist])\/(\d+)-\w+)");
};
