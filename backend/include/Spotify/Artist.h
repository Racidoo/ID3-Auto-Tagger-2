#if !defined(SPOTIFY_ARTIST_H)
#define SPOTIFY_ARTIST_H

#include <sstream>
#include <string>
#include <vector>

#include "SpotifyObject.h"

namespace Spotify {

class Artist : public SpotifyObject {
  private:
  public:
    Artist(const std::string &_id, const std::string &_name,
           const std::string &_imageURL);
    ~Artist();

    static std::string vecToStr(const std::vector<Artist> &_artists,
                                const std::string &_sep = ", ");
};

} // namespace Spotify
#endif // SPOTIFY_ARTIST_H
