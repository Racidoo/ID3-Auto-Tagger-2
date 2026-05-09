#include "Discogs/Release.h"

namespace Discogs {

Release::Release(int _id, const std::string &_name,
                 const std::string &_imageUrl, std::vector<Artist> _artists,
                 const std::string &_country,
                 const std::vector<std::string> &_genres,
                 const std::vector<Label> &_labels,
                 const std::string &_releaseDate,
                 const std::vector<std::string> &_styles,
                 const std::vector<Track> &_tracklist, int _year,
                 const std::vector<Video> &_videos, bool _verified)
    : MasterRelease(_id, _name, _imageUrl, _artists, _genres, _styles,
                    _tracklist, _year, _videos),
      country(_country), labels(_labels), releaseDate(_releaseDate),
      verified(_verified) {}

const std::string &Release::get_country() const { return country; }
const std::vector<Label> &Release::get_labels() const { return labels; }
const std::string &Release::get_releaseDate() const { return releaseDate; }
bool Release::isVerified() const { return verified; }

} // namespace Discogs