#if !defined(SPOTIFY_ALBUM_H)
#define SPOTIFY_ALBUM_H

#include <string>
#include <vector>

#include "Artist.h"
#include "SpotifyObject.h"

namespace Spotify {

class Album : public SpotifyObject {
  private:
     std::string albumType;
     unsigned int totalTracks;
     std::string releaseDate;
    std::vector<Artist> artists;

    // optional attributes
    std::string label;
    std::string copyright;

  public:
    Album(const std::string &_id, const std::string &_name,
          const std::string &_albumType, const unsigned int _totalTracks,
          const std::string &_releaseDate, const std::string &_imageURL,
          const std::vector<Artist> &_artists);
    ~Album();

    inline const std::string get_releaseDate() const { return releaseDate; }
    inline const std::string get_label() const { return label; }
    inline const std::string get_copyright() const { return copyright; }
    inline const std::string get_artists() const {
        return Artist::vecToStr(artists);
    }

    inline void set_label(const std::string &_label) { label = _label; }
    inline void set_copyright(const std::string &_copyright) {
        copyright = _copyright;
    }
};

} // namespace Spotify
#endif // SPOTIFY_ALBUM_H
