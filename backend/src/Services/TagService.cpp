#include "Services/TagService.h"
#include "Services/TrackIdentificationService.h"
#include "Spotify/SpotifyAPI.h"

const std::unordered_map<IMediaService::MediaSourceId, TagService::SourcePolicy>
    TagService::policies = {
        {IMediaService::MediaSourceId::Discogs,
         {{}, {TagField::AlbumCover, TagField::Genre, TagField::Label}}},
        {IMediaService::MediaSourceId::Local,
         {{TagField::Title, TagField::Artist, TagField::Album,
           TagField::AlbumArtist, TagField::Year, TagField::TrackNumber,
           TagField::DiscNumber, TagField::Copyright, TagField::Genre,
           TagField::Label},
          {TagField::AlbumCover}}},
        {IMediaService::MediaSourceId::Spotify,
         {{TagField::Title, TagField::Artist, TagField::Album,
           TagField::AlbumArtist, TagField::Year, TagField::TrackNumber,
           TagField::DiscNumber, TagField::Copyright, TagField::Label},
          {TagField::AlbumCover, TagField::Genre}}},
};

const std::vector<std::unique_ptr<TagService::ITrackFieldDescriptor>>
    TagService::trackFields = [] {
        std::vector<std::unique_ptr<ITrackFieldDescriptor>> fields;

        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Title, "title",
                [](std::shared_ptr<ITrack> t) { return t->get_name(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) { t->set_name(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Artist, "artist",
                [](std::shared_ptr<ITrack> t) { return t->get_artist(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) { t->set_artist(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Album, "album",
                [](std::shared_ptr<ITrack> t) { return t->get_albumName(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) {
                    t->set_albumName(v);
                }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::AlbumArtist, "albumArtist",
                [](std::shared_ptr<ITrack> t) { return t->get_albumArtist(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) {
                    t->set_albumArtist(v);
                }));
        fields.push_back(
            std::make_unique<
                TrackFieldDescriptor<std::optional<std::vector<std::byte>>>>(
                TagField::AlbumCover, "albumCover",
                [](std::shared_ptr<ITrack> t) { return t->get_image(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::vector<std::byte>> &v) {
                    t->set_image(v);
                }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Genre, "genre",
                [](std::shared_ptr<ITrack> t) { return t->get_genre(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) { t->set_genre(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Copyright, "copyright",
                [](std::shared_ptr<ITrack> t) { return t->get_copyright(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) {
                    t->set_copyright(v);
                }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<size_t>>>(
                TagField::Year, "year",
                [](std::shared_ptr<ITrack> t) { return t->get_year(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<size_t> &v) { t->set_year(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Label, "label",
                [](std::shared_ptr<ITrack> t) { return t->get_label(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) { t->set_label(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<size_t>>>(
                TagField::TrackNumber, "trackNumber",
                [](std::shared_ptr<ITrack> t) { return t->get_trackNumber(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<size_t> &v) { t->set_trackNumber(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<size_t>>>(
                TagField::DiscNumber, "discNumber",
                [](std::shared_ptr<ITrack> t) { return t->get_discNumber(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<size_t> &v) { t->set_discNumber(v); }));
        return fields;
    }();

std::string
TagService::researchMissingTags(std::shared_ptr<ITrackMutable> _track,
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
        return {};
    }
    std::pair<const double, std::shared_ptr<ITrack>> *highestScore = nullptr;
    for (auto &&source : merged.sources) {
        if (Spotify::SpotifyAPI::isValidIdFormat(source.second->get_id())) {
            if (!highestScore || highestScore->first < source.first) {
                highestScore = &source;
            }
        }

        source.second->ensureLoaded();
        if (!source.second->get_image().has_value()) {
            std::cout << "get empty image!" << std::endl;
        } else {
            std::cout << "image present!" << std::endl;
        }
        if (auto mutableTrack =
                std::dynamic_pointer_cast<ITrackMutable>(merged.primary)) {
            applyDifferences(mutableTrack, source.second);
        }
    }
    if (highestScore) {
        return highestScore->second->get_id();
    }

    return {};
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

void TagService::applyDifferences(std::shared_ptr<ITrackMutable> _current,
                                  std::shared_ptr<ITrack> _incoming) {

    auto it = policies.find(_incoming->get_mediaSourceId());
    if (it == policies.end()) {
        std::cerr << "[" << __FILE__ << ":" << __LINE__
                  << "]: Unsupported mediaSource!" << std::endl;
        return;
    }
    const auto &policy = it->second;

    for (const auto &desc : trackFields) {
        desc->apply(_current, _incoming, policy);
    }
}
