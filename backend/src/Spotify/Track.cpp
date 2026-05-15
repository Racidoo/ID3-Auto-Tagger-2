#include "Spotify/Track.h"
#include "Query.h"

namespace Spotify {

Track::Track(const std::string &_id, const std::string &_name, State _state,
             const unsigned int _discNumber, const unsigned long _durationMs,
             const bool _explicitContent, const unsigned int _trackNumber,
             const Album &_album, const std::vector<Artist> &_artists)
    : MediaEntityBase(_id, _name, _state, ""), discNumber(_discNumber),
      durationMs(_durationMs), explicitContent(_explicitContent),
      trackNumber(_trackNumber), album(_album), artists(_artists), genre{},
      downloaded(false) {}

const std::vector<Artist> &Track::get_artists() const { return artists; }
std::vector<Artist> &Track::get_artists() { return artists; }
Album &Track::get_album() { return album; }
const Album &Track::get_album() const { return album; }
std::string Track::get_stringArtists() const { return vecToStr(artists); }
unsigned int Track::get_trackNumber() const { return trackNumber; }
unsigned int Track::get_discNumber() const { return discNumber; }
unsigned int Track::get_durationMs() const { return durationMs; }
const std::string &Track::get_genre() const { return genre; }

bool Track::isDownloaded() const { return downloaded; }

void Track::set_genre(const std::string &_genre) { genre = _genre; }
void Track::set_downloaded(bool _downloaded) { downloaded = _downloaded; }

// void Track::loadAdditionalData(std::weak_ptr<IMediaService> _service) {
//     if (state == MetadataState::Full)
//         return;

//     _service.lock()->load(*this);
//     state = MetadataState::Full;
// }

} // namespace Spotify