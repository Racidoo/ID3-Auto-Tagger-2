#pragma once

#include <string>
#include <vector>

#include "Discogs/Artist.h"
#include "Discogs/Label.h"
#include "QueryObject.h"

namespace Discogs {
class ReleaseTrack;
class Release : public QueryObject {
  public:
    // struct Track {
    //     std::string duration;
    //     std::string position;
    //     std::string title;
    //     std::vector<Artist> extraArtists;
    // };
    struct Video {
        std::string description;
        std::string title;
        std::string uri;
        int duration;
    };

    Release(int _id, const std::string &_name, MetadataState _state,
            const std::string &_imageUrl, std::vector<Artist> _artists,
            int _masterId, const std::string &_country,
            const std::vector<std::string> &_genres,
            const std::vector<Label> &_labels, const std::string &_releaseDate,
            const std::vector<std::string> &_styles, int _year,
            const std::vector<Video> &_videos, bool _verified);
    ~Release() = default;

    const std::vector<Artist> &get_artists() const;
    std::string get_stringArtists() const;
    const std::vector<std::string> &get_genres() const;
    std::string get_stringGenres() const;
    const std::vector<std::string> &get_styles() const;
    std::string get_stringStyles() const;
    const std::vector<ReleaseTrack> &get_tracklist() const;
    int get_year() const;
    const std::vector<Video> &get_videos() const;
    int get_masterId() const;
    const std::string &get_country() const;
    const std::vector<Label> &get_labels() const;
    std::string get_stringLabels() const;
    const std::string &get_releaseDate() const;
    bool isVerified() const;

    void set_tracklist(const std::vector<ReleaseTrack> &_tracklist);

  private:
    std::vector<Artist> artists;
    std::vector<std::string> genres;
    std::vector<std::string> styles;
    std::vector<ReleaseTrack> tracklist;
    int year;
    std::vector<Video> videos;
    int masterId;
    std::string country;
    std::vector<Label> labels;
    std::string releaseDate;
    bool verified;
};

} // namespace Discogs
