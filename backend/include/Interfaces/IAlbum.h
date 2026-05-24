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
    virtual std::optional<std::string> get_artist() const = 0;
    virtual std::optional<std::vector<std::byte>> get_artistImage() = 0;
    virtual std::optional<std::size_t> get_year() const = 0;
    virtual std::optional<std::string> get_label() const = 0;
    virtual std::optional<std::string> get_copyright() const = 0;
    virtual const std::vector<std::shared_ptr<ITrack>> &
    get_tracklist() const = 0;
};
