#if !defined(SPOTIFY_SPOTIFY_OBJECT_H)
#define SPOTIFY_SPOTIFY_OBJECT_H

#include <string>

namespace Spotify {

class SpotifyObject {
  protected:
    const std::string id;
    const std::string name;
    const std::string type;

  public:
    SpotifyObject(const std::string &_id, const std::string &_name,
                  const std::string &_type);
    ~SpotifyObject();

    inline const std::string &get_id() const { return id; }
    inline const std::string &get_name() const { return name; }
    inline const std::string &get_type() const { return type; }
};

} // namespace Spotify

#endif // SPOTIFY_SPOTIFY_OBJECT_H
