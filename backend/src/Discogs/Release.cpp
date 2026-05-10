#include "Discogs/Release.h"

namespace Discogs {

Release::Release(int _id, const std::string &_name, MetadataState _state,
                 const std::string &_imageUrl, std::vector<Artist> _artists,
                 int _masterId, const std::string &_country,
                 const std::vector<std::string> &_genres,
                 const std::vector<Label> &_labels,
                 const std::string &_releaseDate,
                 const std::vector<std::string> &_styles,
                 const std::vector<Track> &_tracklist, int _year,
                 const std::vector<Video> &_videos, bool _verified)
    : MasterRelease(_id, _name, _state, _imageUrl, _artists, _genres, _styles,
                    _tracklist, _year, _videos),
      masterId(_masterId), country(_country), labels(_labels),
      releaseDate(_releaseDate), verified(_verified) {}

int Release::get_masterId() const { return masterId; }
const std::string &Release::get_country() const { return country; }
const std::vector<Label> &Release::get_labels() const { return labels; }
std::string Release::get_stringLabels() const {
    return Label::vecToStr(labels);
}
const std::string &Release::get_releaseDate() const { return releaseDate; }
bool Release::isVerified() const { return verified; }

} // namespace Discogs