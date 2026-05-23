#pragma once

#include <functional>
#include <memory>

#include "Interfaces/ITrack.h"
#include "Services/SearchService.h"

class TagService {
  public:
    // TagService(/* args */);
    // ~TagService() = default;

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

    // struct FieldPolicy {
    //     bool allowOverwriteEmptyOnly = false;
    //     bool allowOverwriteAlways = false;
    // };

    // struct TagUpdate {
    //     TagField field;

    //     std::string oldValue;
    //     std::string newValue;

    //     IMediaService::MediaSourceId source;

    //     bool accepted = false;
    // };

    struct SourcePolicy {
        std::unordered_set<TagField> trustedFields;
        std::unordered_set<TagField> emptyOnlyFields;
    };

    struct ITrackFieldDescriptor {
        virtual ~ITrackFieldDescriptor() = default;

        virtual void apply(std::shared_ptr<ITrack> _target,
                           std::shared_ptr<ITrack> _source,
                           const SourcePolicy &_policy) const = 0;
    };

    template <typename T> struct TrackFieldDescriptor : ITrackFieldDescriptor {
        TagField field;
        const char *name;

        std::function<T(std::shared_ptr<ITrack>)> getter;
        std::function<void(std::shared_ptr<ITrack>, const T &)> setter;

        TrackFieldDescriptor(
            TagField _field, const char *_name,
            std::function<T(std::shared_ptr<ITrack>)> _getter,
            std::function<void(std::shared_ptr<ITrack>, const T &)> _setter)
            : field(_field), name(_name), getter(std::move(_getter)),
              setter(std::move(_setter)) {}

        void apply(std::shared_ptr<ITrack> _target,
                   std::shared_ptr<ITrack> _source,
                   const SourcePolicy &_policy) const override {
            auto current = getter(_target);
            auto incoming = getter(_source);
            std::cout << "incoming image size: " << incoming.size()
                      << std::endl;
            if (!TagService::shouldApply(field, current, incoming, _policy))
                return;

            std::cout << "Changed " << name << "\n";
            setter(_target, incoming);
        }
    };

    static bool researchMissingTags(std::shared_ptr<ITrack> _track,
                                    SearchService *_searchService);
    static std::string researchVideoId(std::shared_ptr<ITrack> _track,
                                       SearchService *_searchService);
    static void applyDifferences(std::shared_ptr<ITrack> _current,
                                 std::shared_ptr<ITrack> _incoming);
    template <typename T>
    static bool shouldApply(TagField _field, const T &_current,
                            const T &_incoming, const SourcePolicy &_policy) {
        if (_incoming.empty())
            return false;

        if (_policy.trustedFields.find(_field) != _policy.trustedFields.end())
            return _current != _incoming;

        if (_policy.emptyOnlyFields.find(_field) !=
            _policy.emptyOnlyFields.end())
            return _current.empty() && _current != _incoming;

        return false;
    }

  private:
    static const std::unordered_map<IMediaService::MediaSourceId, SourcePolicy>
        policies;
    static const std::vector<std::unique_ptr<ITrackFieldDescriptor>>
        trackFields;
};