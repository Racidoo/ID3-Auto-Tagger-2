#pragma once

#include <string>
#include <vector>

#include "IMediaSource.hpp"

class ITrack;

class IAlbumSource : public IMediaSource {
  public:
    virtual ~IAlbumSource() = default;

    virtual std::string get_artist() = 0;
    virtual std::string get_copyright() = 0;
    virtual std::string get_genre() = 0;
    virtual std::string get_year() = 0;
    virtual std::string get_label() = 0;

    virtual std::string get_type() const = 0;
    virtual std::vector<std::byte> get_artistCover() = 0;
    virtual std::vector<std::shared_ptr<ITrack>> get_tracklist() const = 0;

    virtual void set_artist(const std::string &_artist) = 0;
    virtual void set_copyright(const std::string &_copyright) = 0;
    virtual void set_genre(const std::string &_genre) = 0;
    virtual void set_year(const std::string &_year) = 0;
    virtual void set_label(const std::string &_label) = 0;
};
