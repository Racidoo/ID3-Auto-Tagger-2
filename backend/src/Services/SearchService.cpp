#include "Services/SearchService.h"
#include "Services/Sources/IMediaSource.hpp"
#include "Services/Sources/SpotifyMediaSource.h"
#include "Services/TrackIdentificationService.h"
#include "Spotify/SpotifyAPI.h"

SearchService::SearchService(std::vector<IMediaSource *> _mediaSources)
    : mediaSources(std::move(_mediaSources)) {}

std::vector<ISearchResult>
SearchService::search(std::shared_ptr<ITrack> _track,
                      const SearchOptions &_options) {
    std::vector<ISearchResult> results;
    if (!_track->get_name().has_value() && !_track->get_artist().has_value() &&
        _track->get_id().empty())
        return {};

    std::string query;

    if (!_track->get_albumName().has_value() &&
        !_track->get_name().has_value()) {
        query = _track->get_id();
    } else if (_track->get_artist().has_value()) {
        query = _track->get_artist().value();
    }
    if (_track->get_albumName().has_value()) {
        query += " - " + _track->get_albumName().value();
    } else {
        query += _track->get_name().value();
    }

    for (auto mediaSource : mediaSources) {
        if (!_options.sources.empty() &&
            _options.sources.find(mediaSource->get_id()) ==
                _options.sources.end()) {
            std::cerr << "unable to find mediaSourceId: "
                      << static_cast<int>(mediaSource->get_id()) << std::endl;
            continue;
        }
        // if (dynamic_cast<SpotifyMediaSource *>(mediaSource)) {
        //     if (Spotify::SpotifyAPI::isValidIdFormat(_track->get_id())) {
        //         ISearchResult result;
        //         result.tracks.push_back(
        //             mediaSource->getTrack(_track->get_id()));
        //         // return {result};
        //         continue;
        //     }
        // }
        results.push_back(mediaSource->search(query, _options));
    }
    return results;
}

std::vector<ISearchResult>
SearchService::search(const std::string &_query,
                      const SearchOptions &_options) {
    std::vector<ISearchResult> results;
    for (auto mediaSource : mediaSources) {
        if (!_options.sources.empty() &&
            _options.sources.find(mediaSource->get_id()) ==
                _options.sources.end()) {
            continue;
        }
        results.push_back(mediaSource->search(_query, _options));
    }

    auto agg = TrackIdentificationService::aggregate(results);

    for (auto &&aggTrack : agg) {
        std::cout << aggTrack.primary->get_name().value() << " - "
                  << aggTrack.primary->get_artist().value()
                  << "\nIDs:" << std::endl;
        for (auto &&source : aggTrack.sources) {
            std::cout << "\t" << source.second->get_id() << std::endl;
        }
    }

    return results;
}

ISearchResult SearchService::resolve(const std::string &_url) {
    for (auto mediaSource : mediaSources) {
        if (mediaSource->supports(_url)) {
            return mediaSource->resolve(_url);
        }
    }
    return {};
}

bool SearchService::supports(const std::string &_url) {
    for (auto mediaSource : mediaSources) {
        if (mediaSource->supports(_url)) {
            return true;
        }
    }
    return false;
}