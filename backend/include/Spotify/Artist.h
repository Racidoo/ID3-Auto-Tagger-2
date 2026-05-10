#if !defined(SPOTIFY_ARTIST_H)
#define SPOTIFY_ARTIST_H

#include <sstream>
#include <string>
#include <vector>

#include "QueryObject.h"

namespace Spotify {

class Artist : public QueryObject {
  private:
  public:
    Artist(const std::string &_id, const std::string &_name,
           MetadataState _state, const std::string &_imageURL);
    ~Artist() = default;
};

} // namespace Spotify
#endif // SPOTIFY_ARTIST_H
