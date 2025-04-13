#if !defined(SPOTIFY_USER_H)
#define SPOTIFY_USER_H

#include "SpotifyObject.h"

namespace Spotify {
class User : public SpotifyObject {
  private:
    /* data */
  public:
    User(const std::string &_id, const std::string &_name,
         const std::string &_imageURL);
    ~User();
};

} // namespace Spotify

#endif // SPOTIFY_USER_H
