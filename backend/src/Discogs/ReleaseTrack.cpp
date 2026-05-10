#include "Discogs/ReleaseTrack.h"
#include "Query.h"

namespace Discogs {

ReleaseTrack::ReleaseTrack(const std::string &_id, const std::string &_title,
                           const Release &_release,
                           const std::vector<Artist> &_artists,
                           const std::vector<Artist> &_extraArtists,
                           const std::vector<std::string> &_genres,
                           const std::string &_imageUrl,
                           const std::vector<Label> &_labels,
                           int _duration, const std::string &_position,
                           int _year)
    : id(_id), title(_title), release(_release), artists(_artists),
      extraArtists(_extraArtists), genres(_genres), imageUrl(_imageUrl),
      labels(_labels), duration(_duration),
      position(_position), year(_year) {}

const std::string &ReleaseTrack::get_id() const { return id; }
const std::string &ReleaseTrack::get_title() const { return title; }
const Release &ReleaseTrack::get_release() const { return release; }
const std::vector<Artist> &ReleaseTrack::get_artists() const { return artists; }
const std::vector<Artist> &ReleaseTrack::get_extraArtists() const {
    return extraArtists;
}
std::string ReleaseTrack::get_stringArtists() const {
    std::string artists;
    std::vector<Artist> allArtists(get_artists());
    allArtists.insert(allArtists.end(), get_extraArtists().begin(),
                      get_extraArtists().end());
    return Artist::vecToStr(allArtists);
}
const std::vector<std::string> &ReleaseTrack::get_genres() const {
    return genres;
}
std::string ReleaseTrack::get_stringGenres() const {
    std::string genres;
    for (auto &&genre : get_genres()) {
        if (!genres.empty())
            genres += "\n";
        genres += genre;
    }
    return genres;
}
const std::string &ReleaseTrack::get_imageUrl() const { return imageUrl; }
std::vector<std::byte> ReleaseTrack::get_image() const {
    return Query::downloadImage(get_imageUrl());
}
const std::vector<Label> &ReleaseTrack::get_labels() const { return labels; }
std::string ReleaseTrack::get_stringLabels() const {
    return Label::vecToStr(labels);
}
int ReleaseTrack::get_duration() const { return duration; }
const std::string &ReleaseTrack::get_position() const { return position; }
int ReleaseTrack::get_year() const { return year; }

int ReleaseTrack::parseDuration(std::string _stringDuration) {
    // 1:12:35
    auto posSecond = _stringDuration.find_last_of(':');
    int s = std::stoi(_stringDuration.substr(posSecond, 2));
    _stringDuration =
        _stringDuration.substr(0, _stringDuration.size() - posSecond);
    auto posMinute = _stringDuration.find_last_of(':');
    int m(0), h(0);
    if (posMinute == std::string::npos) {
        m = std::stoi(_stringDuration);
    } else {
        m = std::stoi(_stringDuration.substr(posMinute, 2));
        _stringDuration =
            _stringDuration.substr(0, _stringDuration.size() - posMinute);
        h = std::stoi(_stringDuration);
    }
    return s + (m * 60) + (h * 3600);
}

} // namespace Discogs