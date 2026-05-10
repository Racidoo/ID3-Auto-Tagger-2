#pragma once

#include <string>
#include <vector>

#include "Discogs/Artist.h"
#include "Discogs/Label.h"
#include "Discogs/MasterRelease.h"

namespace Discogs {
class Release : public MasterRelease {
  public:
    Release(int _id, const std::string &_name, MetadataState _state,
            const std::string &_imageUrl, std::vector<Artist> _artists,
            int _masterId, const std::string &_country,
            const std::vector<std::string> &_genres,
            const std::vector<Label> &_labels, const std::string &_releaseDate,
            const std::vector<std::string> &_styles,
            const std::vector<Track> &_tracklist, int _year,
            const std::vector<Video> &_videos, bool _verified);
    ~Release() = default;

    int get_masterId() const;
    const std::string &get_country() const;
    const std::vector<Label> &get_labels() const;
    std::string get_stringLabels() const;
    const std::string &get_releaseDate() const;
    bool isVerified() const;

  private:
    int masterId;
    std::string country;
    std::vector<Label> labels;
    std::string releaseDate;
    bool verified;
};

} // namespace Discogs