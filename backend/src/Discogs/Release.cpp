#include "Discogs/Release.h"
#include "Discogs/ReleaseTrack.h"
#include "Interfaces/IArtist.h"
#include "Interfaces/ITrack.h"

namespace Discogs {

Release::Release(int _id, const std::string &_name, State _state,
                 const std::string &_imageUrl,
                 const std::vector<std::shared_ptr<IArtist>> &_artists,
                 int _masterId, IAlbum::album_type_t _type,
                 // const std::string &_country,
                 const std::vector<std::string> &_genres,
                 const std::vector<Label> &_labels,
                 const std::string &_copyright,
                 //  const std::string &_releaseDate,
                 //  const std::vector<std::string> &_styles,
                 std::size_t _year, const std::vector<Video> &_videos,
                 bool _verified, IMediaService *_mediaService)
    : IAlbum(std::to_string(_id), _name, _state, _imageUrl, _mediaService),
      artists(_artists), genres(_genres),
      //   styles(_styles),
      year(_year), videos(_videos), masterId(_masterId), type(_type),
      //   country(_country),
      labels(_labels), copyright(_copyright),
      //   releaseDate(_releaseDate),
      verified(_verified) {
    for (auto &&genre : _genres) {
        if (!stringGenres.empty())
            stringGenres += "\n";
        stringGenres += genre;
    }
    for (auto &&label : _labels) {
        if (!stringLabels.empty())
            stringLabels += "\n";
        stringLabels += label.get_name();
    }
}

IAlbum::album_type_t Release::get_type() const { return type; }
std::string Release::get_artist() const { return vecToStr(artists); }
std::vector<std::byte> Release::get_artistImage() {
    if (artists.empty()) {
        return std::vector<std::byte>{};
    }
    return artists[0]->get_image();
}
std::size_t Release::get_year() const { return year; }
const std::string &Release::get_label() const { return stringLabels; }
const std::string &Release::get_copyright() const { return copyright; }
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
const std::string &Release::get_stringGenres() const { return stringGenres; }
// const std::vector<std::string> &Release::get_styles() const { return styles;
// } std::string Release::get_stringStyles() const {
//     std::string styles;
//     for (auto &&style : get_styles()) {
//         if (!styles.empty())
//             styles += "\n";
//         styles += style;
//     }
//     return styles;
// }
const std::vector<Release::Video> &Release::get_videos() const {
    return videos;
}
int Release::get_masterId() const { return masterId; }
// const std::string &Release::get_country() const { return country; }
const std::vector<Label> &Release::get_labels() const { return labels; }
std::string Release::get_stringLabels() const {
    return Label::vecToStr(labels);
}
// const std::string &Release::get_releaseDate() const { return releaseDate; }
bool Release::isVerified() const { return verified; }

void Release::set_year(std::size_t _year) { year = _year; }
void Release::set_copyright(const std::string &_copyright) {
    copyright = _copyright;
}
void Release::set_label(const std::string &_label) {
    static_assert("Discogs Object is not mutable!");
}
void Release::set_tracklist(
    const std::vector<std::shared_ptr<ITrack>> &_tracklist) {
    tracklist = _tracklist;
}

void Release::set_artists(std::vector<std::shared_ptr<IArtist>> _artists) {
    artists = _artists;
}

} // namespace Discogs