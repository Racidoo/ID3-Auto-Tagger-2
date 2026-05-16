#include "Discogs/Release.h"
#include "Discogs/ReleaseTrack.h"
#include "Interfaces/IArtist.h"
#include "Interfaces/ITrack.h"

namespace Discogs {

Release::Release(int _id, const std::string &_name, State _state,
                 const std::string &_imageUrl,
                 const std::vector<std::shared_ptr<IArtist>> &_artists,
                 int _masterId, const std::string &_country,
                 const std::vector<std::string> &_genres,
                 const std::vector<Label> &_labels,
                 const std::string &_copyright, const std::string &_releaseDate,
                 const std::vector<std::string> &_styles, std::size_t _year,
                 const std::vector<Video> &_videos, bool _verified)
    : IAlbum(std::to_string(_id), _name, _state, _imageUrl), artists(_artists),
      genres(_genres), styles(_styles), year(_year), videos(_videos),
      masterId(_masterId), country(_country), labels(_labels),
      copyright(_copyright), releaseDate(_releaseDate), verified(_verified) {}

const std::string &Release::get_type() const { return type; }
std::string Release::get_artist() const { return vecToStr(artists); }
const std::vector<std::byte> &Release::get_artistImage() {
    return artists[0]->get_image();
}
std::size_t Release::get_year() const { return year; }
const std::string &Release::get_label() const { return labels[0].get_name(); }
const std::string &Release::get_copyright() const { return copyright; }
const std::vector<std::shared_ptr<ITrack>> &Release::get_tracklist() const {
    return tracklist;
}
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
const std::vector<std::shared_ptr<IArtist>> &Release::get_artists() const {
    return artists;
}
std::vector<std::shared_ptr<IArtist>> &Release::get_artists() {
    return artists;
}
const std::vector<std::string> &Release::get_genres() const { return genres; }
std::string Release::get_stringGenres() const {
    std::string genres;
    for (auto &&genre : get_genres()) {
        if (!genres.empty())
            genres += "\n";
        genres += genre;
    }
    return genres;
}
const std::vector<std::string> &Release::get_styles() const { return styles; }
std::string Release::get_stringStyles() const {
    std::string styles;
    for (auto &&style : get_styles()) {
        if (!styles.empty())
            styles += "\n";
        styles += style;
    }
    return styles;
}
const std::vector<Release::Video> &Release::get_videos() const {
    return videos;
}
int Release::get_masterId() const { return masterId; }
const std::string &Release::get_country() const { return country; }
const std::vector<Label> &Release::get_labels() const { return labels; }
std::string Release::get_stringLabels() const {
    return Label::vecToStr(labels);
}
const std::string &Release::get_releaseDate() const { return releaseDate; }
bool Release::isVerified() const { return verified; }

} // namespace Discogs