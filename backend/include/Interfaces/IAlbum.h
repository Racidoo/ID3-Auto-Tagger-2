#pragma once

#include "MediaEntityBase.h"

class ITrack;

class IAlbum : public MediaEntityBase {
  public:
    explicit IAlbum(const std::string &_id, State _state,
                    IMediaService *_mediaService)
        : MediaEntityBase(_id, _state, _mediaService) {}
    ~IAlbum() = default;

    enum album_type_t { UNDEFINED, ALBUM, SINGLE, COMPILATION };

    virtual album_type_t get_type() const = 0;
    virtual std::string get_artist() const = 0;
    virtual std::vector<std::byte> get_artistImage() = 0;
    virtual std::size_t get_year() const = 0;
    virtual const std::string &get_label() const = 0;
    virtual const std::string &get_copyright() const = 0;
    virtual const std::vector<std::shared_ptr<ITrack>> &
    get_tracklist() const = 0;

    virtual void set_year(std::size_t _year) = 0;
    virtual void set_copyright(const std::string &_copyright) = 0;
    virtual void set_label(const std::string &_label) = 0;
    virtual void
    set_tracklist(const std::vector<std::shared_ptr<ITrack>> &_tracklist) = 0;
};
