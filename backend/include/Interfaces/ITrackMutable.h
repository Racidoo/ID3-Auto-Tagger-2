#pragma once

#include "Interfaces/ITrack.h"

class ITrackMutable : public ITrack {
  public:
    explicit ITrackMutable(const std::string &_id, State _state,
                           IMediaService *_mediaService)
        : ITrack(_id, _state, _mediaService) {}
    ~ITrackMutable() = default;

    virtual void set_name(const std::optional<std::string> &_name) = 0;
    virtual void set_artist(const std::optional<std::string> &_artist) = 0;
    virtual void
    set_albumName(const std::optional<std::string> &_albumName) = 0;
    virtual void
    set_albumArtist(const std::optional<std::string> &_albumArtist) = 0;
    virtual void
    set_copyright(const std::optional<std::string> &_copyright) = 0;
    virtual void set_genre(const std::optional<std::string> &_genre) = 0;
    virtual void set_year(std::optional<size_t> _year) = 0;
    virtual void set_label(const std::optional<std::string> &_label) = 0;
    virtual void set_trackNumber(std::optional<size_t> _trackNumber) = 0;
    virtual void set_discNumber(std::optional<size_t> _discNumber) = 0;
    virtual void
    set_image(const std::optional<std::vector<std::byte>> &_imageDate) = 0;
};
