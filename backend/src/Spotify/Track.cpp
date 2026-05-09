#include "../include/Spotify/Track.h"
#include "../include/Query.h"

namespace Spotify {

Track::Track(const std::string &_id, const std::string &_name,
             const unsigned int _discNumber, const unsigned long _durationMs,
             const bool _explicitContent, const unsigned int _trackNumber,
             const Album &_album, const std::vector<Artist> &_artists)
    : QueryObject(_id, _name, "track", ""), discNumber(_discNumber),
      durationMs(_durationMs), explicitContent(_explicitContent),
      trackNumber(_trackNumber), album(_album), artists(_artists),
      genre{}, downloaded(false) {}

const std::vector<Artist> &Track::get_artists() const { return artists; }
std::vector<Artist> &Track::get_artists() { return artists; }
Album &Track::get_album() { return album; }
const Album &Track::get_album() const { return album; }
std::string Track::get_stringArtists() const {
    return Artist::vecToStr(artists);
}
unsigned int Track::get_trackNumber() const { return trackNumber; }
unsigned int Track::get_discNumber() const { return discNumber; }
unsigned int Track::get_durationMs() const { return durationMs; }
const std::string &Track::get_genre() const { return genre; }

bool Track::isDownloaded() const { return downloaded; }

void Track::set_genre(const std::string &_genre) { genre = _genre; }
void Track::set_downloaded(bool _downloaded) { downloaded = _downloaded; }

} // namespace Spotify