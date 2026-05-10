#pragma once

#include <string>
#include <vector>

#include "Discogs/Artist.h"
#include "QueryObject.h"

namespace Discogs {
class MasterRelease : public QueryObject {
  public:
    struct Track {
        std::string duration;
        std::string position;
        std::string title;
    };
    struct Video {
        std::string description;
        std::string title;
        std::string uri;
        int duration;
    };

    MasterRelease(int _id, const std::string &_name, MetadataState _state,
                  const std::string &_imageUrl, std::vector<Artist> _artists,
                  const std::vector<std::string> &_genres,
                  const std::vector<std::string> &_styles,
                  const std::vector<Track> &_tracklist, int _year,
                  const std::vector<Video> &_videos);
    ~MasterRelease() = default;

    const std::vector<Artist> &get_artists() const;
    std::string get_stringArtists() const;
    const std::vector<std::string> &get_genres() const;
    std::string get_stringGenres() const;
    const std::vector<std::string> &get_styles() const;
    const std::vector<Track> &get_tracklist() const;
    int get_year() const;
    const std::vector<Video> &get_videos() const;

  private:
    std::vector<Artist> artists;
    std::vector<std::string> genres;
    std::vector<std::string> styles;
    std::vector<Track> tracklist;
    int year;
    std::vector<Video> videos;
};

} // namespace Discogs
