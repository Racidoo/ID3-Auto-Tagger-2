#pragma once

#include <string>
#include <vector>

#include "Discogs/Artist.h"
#include "Discogs/Label.h"
#include "Discogs/Release.h"

namespace Discogs {

class ReleaseTrack {
  public:
    ReleaseTrack(const std::string &_id, const std::string &_title,
                 const Release &_release, const std::vector<Artist> &_artists,
                 const std::vector<Artist> &_extraArtists,
                 const std::vector<std::string> &_genres,
                 const std::string &_imageUrl,
                 const std::vector<Label> &_labels, int _duration,
                 const std::string &_position, int _year);
    ~ReleaseTrack() = default;

    const std::string &get_id() const;
    const std::string &get_title() const;
    const Release &get_release() const;
    const std::vector<Artist> &get_artists() const;
    const std::vector<Artist> &get_extraArtists() const;
    std::string get_stringArtists() const;
    const std::vector<std::string> &get_genres() const;
    std::string get_stringGenres() const;
    const std::string &get_imageUrl() const;
    std::vector<std::byte> get_image() const;
    const std::vector<Label> &get_labels() const;
    std::string get_stringLabels() const;
    int get_duration() const;
    const std::string &get_position() const;
    int get_year() const;

    static int parseDuration(std::string _stringDuration);

  private:
    std::string id;
    std::string title;
    Release release;
    std::vector<Artist> artists;
    std::vector<Artist> extraArtists;
    std::vector<std::string> genres;
    std::string imageUrl;
    std::vector<Label> labels;
    int duration;
    std::string position;
    int year;
};

} // namespace Discogs