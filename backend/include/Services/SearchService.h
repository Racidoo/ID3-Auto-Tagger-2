#pragma once

#include <unordered_set>

#include "Interfaces/ISearchResult.hpp"
#include "Services/IMediaService.hpp"

class IMediaSource;

class SearchService {
  public:
    SearchService(std::vector<IMediaSource *> _mediaSources);

    std::vector<ISearchResult> search(std::shared_ptr<ITrack> _track,
                                      const SearchOptions &_options = {});
    std::vector<ISearchResult> search(const std::string &_query,
                                      const SearchOptions &_options = {});
    ISearchResult resolve(const std::string &_url);
    bool supports(const std::string &_url);

    IMediaSource *getMediaSource(IMediaService::MediaSourceId _sourceId);

  private:
    std::vector<IMediaSource *> mediaSources;
};
