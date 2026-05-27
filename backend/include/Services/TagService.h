#pragma once

#include <functional>
#include <memory>
#include <optional>

#include "Interfaces/ITrackMutable.h"
#include "Services/SearchService.h"

class TagService {
  public:
    static constexpr double AUTO_ACCEPT_THRESHOLD = 0.80;
    static constexpr double USER_VERIFY_THRESHOLD = 0.60;

    enum class TagField {
        Title,
        Artist,
        Album,
        AlbumArtist,
        AlbumCover,
        Genre,
        Copyright,
        Year,
        Label,
        TrackNumber,
        DiscNumber
    };

    struct AggregatedTrack {
        std::map<double, std::shared_ptr<ITrack>> sources;
        std::map<double, std::shared_ptr<IVideo>> videos;

        std::shared_ptr<ITrack> primary;
    };

    enum class VerificationState {
        Accepted,
        RequiresUserVerification,
        Rejected
    };

    struct VerificationCandidate {
        double score;
        std::shared_ptr<ITrack> track;

        // Optional explanation/debug info
        std::vector<std::string> matchReasons;
    };

    struct TagResearchResult {
        VerificationState state;
        std::shared_ptr<ITrack> acceptedTrack;
        std::vector<VerificationCandidate> candidates;
    };

    struct SourcePolicy {
        std::unordered_set<TagField> trustedFields;
        std::unordered_set<TagField> emptyOnlyFields;
    };

    struct ITrackFieldDescriptor {
        virtual ~ITrackFieldDescriptor() = default;

        virtual void apply(std::shared_ptr<ITrackMutable> _target,
                           std::shared_ptr<ITrack> _source,
                           const SourcePolicy &_policy) const = 0;
    };

    template <typename T> struct TrackFieldDescriptor : ITrackFieldDescriptor {
        TagField field;
        const char *name;

        std::function<T(std::shared_ptr<ITrack>)> getter;
        std::function<void(std::shared_ptr<ITrackMutable>, const T &)> setter;

        TrackFieldDescriptor(
            TagField _field, const char *_name,
            std::function<T(std::shared_ptr<ITrack>)> _getter,
            std::function<void(std::shared_ptr<ITrackMutable>, const T &)>
                _setter)
            : field(_field), name(_name), getter(std::move(_getter)),
              setter(std::move(_setter)) {}

        void apply(std::shared_ptr<ITrackMutable> _target,
                   std::shared_ptr<ITrack> _source,
                   const SourcePolicy &_policy) const override;
    };

    static TagResearchResult
    identifyTrack(std::shared_ptr<ITrackMutable> _track,
                  SearchService *_searchService);
    static std::string researchVideoId(std::shared_ptr<ITrack> _track,
                                       SearchService *_searchService);
    static void applyTagDifferences(std::shared_ptr<ITrackMutable> _current,
                                    std::shared_ptr<ITrack> _incoming);

    static std::vector<AggregatedTrack>
    aggregate(const std::vector<ISearchResult> &_results);

    static AggregatedTrack merge(std::shared_ptr<ITrack> _track,
                                 const std::vector<ISearchResult> &_results);

    static std::string normalizeTrackTitle(std::string _title);

  private:
    template <typename T>
    static bool shouldApply(TagField _field, const std::optional<T> &_current,
                            const std::optional<T> &_incoming,
                            const SourcePolicy &_policy);

    static double scoreTracks(const std::shared_ptr<ITrack> &_a,
                              const std::shared_ptr<ITrack> &_b);
    static double scoreTracks(const std::shared_ptr<IVideo> &_a,
                              const std::shared_ptr<ITrack> &_b);

    static std::string toLower(const std::string &_str);
    static double similarityScore(const std::string &_s1,
                                  const std::string &_s2);
    static double similarityScore(int _duration1, int _duration2);
    static double similarityScoreDate(const std::string &_date1,
                                      const std::string &_date2);
    static int levenshteinDistance(const std::string &_s1,
                                   const std::string &_s2);
    static bool findInString(const std::string &_base,
                             const std::string &_searchText);

    static std::vector<std::string> splitArtists(std::string _artist);

    static double scoreArtistMatch(const std::string &_title,
                                   const std::string &_artist);

    static std::vector<std::string> collaborationSeparators;

    static const std::unordered_map<IMediaService::MediaSourceId, SourcePolicy>
        policies;
    static const std::vector<std::unique_ptr<ITrackFieldDescriptor>>
        trackFields;
};

template <typename T>
void TagService::TrackFieldDescriptor<T>::apply(
    std::shared_ptr<ITrackMutable> _target, std::shared_ptr<ITrack> _source,
    const SourcePolicy &_policy) const {
    auto current = getter(_target);
    auto incoming = getter(_source);
    if (!TagService::shouldApply(field, current, incoming, _policy))
        return;

    std::cout << "Changed " << name << "\n";
    setter(_target, incoming);
}

template <typename T>
bool TagService::shouldApply(TagField _field, const std::optional<T> &_current,
                             const std::optional<T> &_incoming,
                             const SourcePolicy &_policy) {
    if (!_incoming.has_value())
        return false;

    if (_policy.trustedFields.find(_field) != _policy.trustedFields.end())
        return _current != _incoming;

    if (_policy.emptyOnlyFields.find(_field) != _policy.emptyOnlyFields.end())
        return !_current.has_value() && _current != _incoming;

    return false;
}