#include "Discogs/Release.h"

namespace Discogs {

MasterRelease::MasterRelease(int _id, const std::string &_name,
                             MetadataState _state, const std::string &_imageUrl,
                             std::vector<Artist> _artists,
                             const std::vector<std::string> &_genres,
                             const std::vector<std::string> &_styles,
                             const std::vector<Track> &_tracklist, int _year,
                             const std::vector<Video> &_videos)
    : QueryObject(std::to_string(_id), _name, _state, _imageUrl),
      artists(_artists), genres(_genres), styles(_styles),
      tracklist(_tracklist), year(_year) {}

const std::vector<Artist> &MasterRelease::get_artists() const {
    return artists;
}
std::string MasterRelease::get_stringArtists() const {
    return Artist::vecToStr(artists);
}
const std::vector<std::string> &MasterRelease::get_genres() const {
    return genres;
}
std::string MasterRelease::get_stringGenres() const {
    std::string genres;
    for (auto &&genre : get_genres()) {
        if (!genres.empty())
            genres += "\n";
        genres += genre;
    }
    return genres;
}
const std::vector<std::string> &MasterRelease::get_styles() const {
    return styles;
}
const std::vector<Release::Track> &MasterRelease::get_tracklist() const {
    return tracklist;
}
int MasterRelease::get_year() const { return year; }
const std::vector<Release::Video> &MasterRelease::get_videos() const {
    return videos;
}

} // namespace Discogs