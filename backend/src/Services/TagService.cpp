#include "Services/TagService.h"
#include "Services/TrackIdentificationService.h"
#include "Spotify/SpotifyAPI.h"

bool TagService::researchMissingTags(std::shared_ptr<ITrack> _track,
                                     SearchService *_searchService) {

    SearchOptions options;

    options.sources.insert(IMediaService::MediaSourceId::Spotify);
    options.sources.insert(IMediaService::MediaSourceId::Discogs);
    options.categories.insert(ISearchResult::SearchCategory::Track);

    _track->ensureLoaded();

    auto searchResults = _searchService->search(_track, options);

    auto merged = TrackIdentificationService::merge(_track, searchResults);

    if (merged.sources.empty()) {
        std::cerr << "Unable to fetch additional meta-data!" << std::endl;
        return false;
    }

    for (auto &&source : merged.sources) {

        source->ensureLoaded();
        applyDifferences(merged.primary, source);
    }
    return true;
}

std::string TagService::researchVideoId(std::shared_ptr<ITrack> _track,
                                        SearchService *_searchService) {
    SearchOptions options;

    options.sources.insert(IMediaService::MediaSourceId::YouTube);
    options.categories.insert(ISearchResult::SearchCategory::Video);

    _track->ensureLoaded();

    auto searchResults = _searchService->search(_track, options);

    auto merged = TrackIdentificationService::merge(_track, searchResults);

    if (merged.videos.empty()) {
        std::cerr << "Unable to fetch videos for " << _track->get_id() << "!"
                  << std::endl;
        return {};
    }

    return merged.videos[0]->get_id();
}

void TagService::applyDifferences(std::shared_ptr<ITrack> _primary,
                                  std::shared_ptr<ITrack> _incoming) {

    auto it = policies.find(_incoming->get_mediaSourceId());
    if (it == policies.end()) {
        std::cerr << "[" << __FILE__ << ":" << __LINE__
                  << "]: Unsupported mediaSource!" << std::endl;
        return;
    }
    const auto &policy = it->second;

    for (const auto &[field, desc] : trackFields) {
        checkField(field, _primary, _incoming, policy);
    }
    checkField(TagField::AlbumCover, _primary, _incoming, policy);
}

bool TagService::shouldApply(TagField _field, const std::string &_current,
                             const std::string &_incoming,
                             const SourcePolicy &_policy) {

    if (_incoming.empty())
        return false;

    if (_policy.trustedFields.find(_field) != _policy.trustedFields.end())
        return _current != _incoming;

    if (_policy.emptyOnlyFields.find(_field) != _policy.emptyOnlyFields.end())
        return _current.empty() && _current != _incoming;

    return false;
}

bool TagService::shouldApply(TagField _field,
                             const std::vector<std::byte> &_current,
                             const std::vector<std::byte> &_incoming,
                             const SourcePolicy &_policy) {

    if (_incoming.empty())
        return false;

    if (_policy.trustedFields.find(_field) != _policy.trustedFields.end())
        return _current != _incoming;

    if (_policy.emptyOnlyFields.find(_field) != _policy.emptyOnlyFields.end())
        return _current.empty() && _current != _incoming;

    return false;
}

void TagService::checkField(TagField _field, std::shared_ptr<ITrack> _target,
                            std::shared_ptr<ITrack> _source,
                            const SourcePolicy &_policy) {
    // image needs to be handled separately
    if (_field == TagField::AlbumCover) {
        if (!shouldApply(_field, _target->get_image(), _source->get_image(),
                         _policy))
            return;
        std::cout << "Changed albumCover\n";
        _target->set_image(_source->get_image());
        return;
    }

    auto it = trackFields.find(_field);

    if (it == trackFields.end())
        return;

    const auto &desc = it->second;

    auto current = (desc.getter)(_target);
    auto incoming = (desc.getter)(_source);

    if (!shouldApply(_field, current, incoming, _policy))
        return;

    std::cout << "Changed " << desc.name << " from '" << current << "' to '"
              << incoming << "'\n";

    (desc.setter)(_target, incoming);
}
