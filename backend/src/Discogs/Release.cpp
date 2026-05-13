#include "Discogs/Release.h"
#include "Discogs/ReleaseTrack.h"

namespace Discogs {

Release::Release(int _id, const std::string &_name, MetadataState _state,
                 const std::string &_imageUrl, std::vector<Artist> _artists,
                 int _masterId, const std::string &_country,
                 const std::vector<std::string> &_genres,
                 const std::vector<Label> &_labels,
                 const std::string &_releaseDate,
                 const std::vector<std::string> &_styles, int _year,
                 const std::vector<Video> &_videos, bool _verified)
    : QueryObject(std::to_string(_id), _name, _state, _imageUrl),
      artists(_artists), genres(_genres), styles(_styles), year(_year),
      videos(_videos), masterId(_masterId), country(_country), labels(_labels),
      releaseDate(_releaseDate), verified(_verified) {}

const std::vector<Artist> &Release::get_artists() const { return artists; }
std::string Release::get_stringArtists() const {
    return Artist::vecToStr(artists);
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
const std::vector<ReleaseTrack> &Release::get_tracklist() const {
    return tracklist;
}
int Release::get_year() const { return year; }
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

void Release::set_tracklist(const std::vector<ReleaseTrack> &_tracklist) {
    tracklist = _tracklist;
}

} // namespace Discogs