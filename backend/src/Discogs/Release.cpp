#include "Discogs/Release.h"
#include "Discogs/ReleaseTrack.h"
#include "Interfaces/IArtist.h"
#include "Interfaces/ITrack.h"

namespace Discogs {

Release::Release(int _id, const std::optional<std::string> &_name, State _state,
                 const std::string &_imageUrl,
                 const std::vector<std::shared_ptr<IArtist>> &_artists,
                 int _masterId, IAlbum::album_type_t _type,
                 const std::vector<std::string> &_genres,
                 const std::vector<Label> &_labels,
                 const std::string &_copyright,
                 std::optional<std::size_t> _year,
                 const std::vector<Video> &_videos, bool _verified,
                 IMediaService *_mediaService)
    : IAlbum(std::to_string(_id), _state, _mediaService), name(_name),
      imageProvider(_imageUrl), artists(_artists), genres(_genres), year(_year),
      videos(_videos), masterId(_masterId), type(_type), labels(_labels),
      copyright(_copyright), verified(_verified) {}
std::optional<std::string> Release::get_name() const { return name; }
std::optional<std::vector<std::byte>> Release::get_image() {
    return imageProvider.get_image();
}
IAlbum::album_type_t Release::get_type() const { return type; }
std::optional<std::string> Release::get_artist() const {
    return vecToStr(artists);
}
std::optional<std::vector<std::byte>> Release::get_artistImage() {
    if (artists.empty()) {
        return std::vector<std::byte>{};
    }
    return artists[0]->get_image();
}
std::optional<std::size_t> Release::get_year() const { return year; }
std::optional<std::string> Release::get_label() const {
    if (labels.empty()) {
        return std::nullopt;
    }
    std::string stringlabels;
    for (auto &&label : labels) {
        if (!stringlabels.empty()) {
            stringlabels += "\n";
        }
        stringlabels += label.get_name().value();
    }
    return stringlabels;
}
std::optional<std::string> Release::get_copyright() const { return copyright; }
const std::vector<std::shared_ptr<ITrack>> &Release::get_tracklist() const {
    return tracklist;
}

const std::vector<std::shared_ptr<IArtist>> &Release::get_artists() const {
    return artists;
}
std::vector<std::shared_ptr<IArtist>> &Release::get_artists() {
    return artists;
}
const std::vector<std::string> &Release::get_genres() const { return genres; }
std::optional<std::string> Release::get_stringGenres() const {
    if (genres.empty()) {
        return std::nullopt;
    }
    std::string stringGenres;
    for (auto &&genre : genres) {
        if (!stringGenres.empty()) {
            stringGenres += "\n";
        }
        stringGenres += genre;
    }
    return stringGenres;
}
const std::vector<Release::Video> &Release::get_videos() const {
    return videos;
}
int Release::get_masterId() const { return masterId; }
const std::vector<Label> &Release::get_labels() const { return labels; }
bool Release::isVerified() const { return verified; }
const IRemoteImageProvider &Release::get_imageProvider() const {
    return imageProvider;
}

void Release::set_tracklist(
    const std::vector<std::shared_ptr<ITrack>> &_tracklist) {
    tracklist = _tracklist;
}

void Release::set_artists(std::vector<std::shared_ptr<IArtist>> _artists) {
    artists = _artists;
}

} // namespace Discogs