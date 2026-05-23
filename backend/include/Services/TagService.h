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

    struct TrackFieldDescriptor {
        // TagField field;

        const char *name;

        std::function<std::string(std::shared_ptr<ITrack>)> getter;
        std::function<void(std::shared_ptr<ITrack>, const std::string &)>
            setter;
    };

    static bool researchMissingTags(std::shared_ptr<ITrack> _track,
                                    SearchService *_searchService);
    static std::string researchVideoId(std::shared_ptr<ITrack> _track,
                                       SearchService *_searchService);
    static void applyDifferences(std::shared_ptr<ITrack> _current,
                                 std::shared_ptr<ITrack> _incoming);
    static bool shouldApply(TagField _field, const std::string &_current,
                            const std::string &_incoming,
                            const SourcePolicy &_policy);
    static bool shouldApply(TagField _field,
                            const std::vector<std::byte> &_current,
                            const std::vector<std::byte> &_incoming,
                            const SourcePolicy &_policy);
    static void checkField(TagField _field, std::shared_ptr<ITrack> _target,
                           std::shared_ptr<ITrack> _source,
                           const SourcePolicy &_policy);

  private:
    static inline const std::unordered_map<IMediaService::MediaSourceId,
                                           SourcePolicy>
        policies = {
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

    static inline const std::unordered_map<TagField, TrackFieldDescriptor>
        trackFields = {
            {TagField::Title,
             TrackFieldDescriptor{
                 "title",
                 [](std::shared_ptr<ITrack> t) { return t->get_name(); },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_name(v);
                 }}},
            {TagField::Artist,
             TrackFieldDescriptor{
                 "artist",
                 [](std::shared_ptr<ITrack> t) { return t->get_artist(); },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_artist(v);
                 }}},
            {TagField::Album,
             TrackFieldDescriptor{
                 "album",
                 [](std::shared_ptr<ITrack> t) { return t->get_albumName(); },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_albumName(v);
                 }}},
            {TagField::AlbumArtist,
             TrackFieldDescriptor{
                 "albumArtist",
                 [](std::shared_ptr<ITrack> t) { return t->get_albumArtist(); },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_albumArtist(v);
                 }}},
            {TagField::Genre,
             TrackFieldDescriptor{
                 "genre",
                 [](std::shared_ptr<ITrack> t) { return t->get_genre(); },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_genre(v);
                 }}},
            {TagField::Copyright,
             TrackFieldDescriptor{
                 "copyright",
                 [](std::shared_ptr<ITrack> t) { return t->get_copyright(); },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_copyright(v);
                 }}},
            {TagField::Year,
             TrackFieldDescriptor{
                 "year",
                 [](std::shared_ptr<ITrack> t) {
                     return std::to_string(t->get_year());
                 },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_year(std::stoi(v));
                 }}},
            {TagField::Label,
             TrackFieldDescriptor{
                 "label",
                 [](std::shared_ptr<ITrack> t) { return t->get_label(); },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_label(v);
                 }}},
            {TagField::TrackNumber,
             TrackFieldDescriptor{
                 "trackNumber",
                 [](std::shared_ptr<ITrack> t) {
                     return std::to_string(t->get_trackNumber());
                 },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_trackNumber(std::stoi(v));
                 }}},
            {TagField::DiscNumber,
             TrackFieldDescriptor{
                 "discNumber",
                 [](std::shared_ptr<ITrack> t) {
                     return std::to_string(t->get_discNumber());
                 },
                 [](std::shared_ptr<ITrack> t, const std::string &v) {
                     t->set_discNumber(std::stoi(v));
                 }}}};
};
