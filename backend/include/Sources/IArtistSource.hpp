#pragma once

#include <string>
#include <vector>

#include "IMediaSource.hpp"

class IArtistSource : public IMediaSource {
  public:
    virtual ~IArtistSource() = default;

    // virtual std::string get_artist() = 0;
    // virtual std::string get_album() = 0;
    // virtual std::string get_albumArtist() = 0;
    // virtual std::string get_copyright() = 0;
    // virtual std::string get_genre() = 0;
    // virtual std::string get_year() = 0;
    // virtual std::string get_label() = 0;
    // virtual std::string get_trackNumber() = 0;
    // virtual std::string get_discNumber() = 0;
    // virtual std::size_t get_length() = 0;

    // virtual void set_artist(const std::string &_artist) = 0;
    // virtual void set_album(const std::string &_album) = 0;
    // virtual void set_albumArtist(const std::string &_albumArtist) = 0;
    // virtual void set_copyright(const std::string &_copyright) = 0;
    // virtual void set_genre(const std::string &_genre) = 0;
    // virtual void set_year(const std::string &_year) = 0;
    // virtual void set_label(const std::string &_label) = 0;
    // virtual void set_trackNumber(const std::string &_track) = 0;
    // virtual void set_discNumber(const std::string &_disc) = 0;
};
