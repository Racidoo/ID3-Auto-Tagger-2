#if !defined(SPOTIFY_ALBUM_H)
#define SPOTIFY_ALBUM_H

#include <string>
#include <vector>

#include "Artist.h"
#include "QueryObject.h"

namespace Spotify {

class Album : public QueryObject {
  public:
    Album(const std::string &_id, const std::string &_name,
          MetadataState _state, const std::string &_albumType,
          unsigned int _totalTracks, const std::string &_releaseDate,
          const std::string &_imageURL, const std::vector<Artist> &_artists);
    ~Album() = default;

    const std::string &get_albumType() const;
    unsigned int get_totalTracks() const;
    const std::string get_releaseDate() const;
    std::string get_releaseYear() const;
    const std::string &get_label() const;
    const std::string &get_copyright() const;
    std::string get_artists() const;

    void set_label(const std::string &_label);
    void set_copyright(const std::string &_copyright);

  private:
    std::string albumType;
    unsigned int totalTracks;
    std::string releaseDate;
    std::vector<Artist> artists;

    // optional attributes
    std::string label;
    std::string copyright;
};

} // namespace Spotify
#endif // SPOTIFY_ALBUM_H
