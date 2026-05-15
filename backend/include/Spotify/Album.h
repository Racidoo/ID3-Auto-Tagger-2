#pragma once 

#include <string>
#include <vector>

#include "Artist.h"
#include "Interfaces/MediaEntityBase.h"

class MediaService;

namespace Spotify {

class Track;
class Album : public MediaEntityBase {
  public:
    Album(const std::string &_id, const std::string &_name, State _state,
          const std::string &_albumType, unsigned int _totalTracks,
          const std::string &_releaseDate, const std::string &_imageURL,
          const std::vector<Artist> &_artists);
    ~Album() = default;

    const std::string &get_albumType() const;
    unsigned int get_totalTracks() const;
    const std::string get_releaseDate() const;
    std::string get_releaseYear() const;
    const std::string &get_label() const;
    const std::string &get_copyright() const;
    const std::vector<Artist> &get_artists() const;
    std::vector<Artist> &get_artists();
    std::string get_stringArtists() const;
    const std::vector<Track> &get_tracklist() const;

    void set_label(const std::string &_label);
    void set_copyright(const std::string &_copyright);
    void set_tracklist(const std::vector<Track> &_tracklist);

    // void loadAdditionalData(std::weak_ptr<IMediaService> _service) override;

  private:
    std::string albumType;
    unsigned int totalTracks;
    std::string releaseDate;
    std::vector<Artist> artists;

    // optional attributes
    std::string label;
    std::string copyright;
    std::vector<Track> tracklist;
};

} // namespace Spotify