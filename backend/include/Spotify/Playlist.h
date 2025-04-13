#if !defined(SPOTIFY_PLAYLIST_H)
#define SPOTIFY_PLAYLIST_H

#include "SpotifyObject.h"
#include "User.h"

namespace Spotify {
class Playlist : public SpotifyObject {
  private:
    User creator;

  public:
    Playlist(const std::string &_id, const std::string &_name,
             const std::string &_imageURL, const User &_creator);
    ~Playlist();

    inline User get_creator() const { return creator; }
};

} // namespace Spotify

#endif // SPOTIFY_PLAYLIST_H
