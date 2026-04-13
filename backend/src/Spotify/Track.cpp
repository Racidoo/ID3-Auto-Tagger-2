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
      downloaded(false) {}

Track::~Track() {}

} // namespace Spotify