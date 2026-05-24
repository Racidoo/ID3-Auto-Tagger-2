#pragma once

#include <memory>
#include <optional>

#include "MediaEntityBase.h"

class ITrack : public MediaEntityBase {
  public:
    explicit ITrack(const std::string &_id, State _state,
                    IMediaService *_mediaService)
        : MediaEntityBase(_id, _state, _mediaService) {}
    ~ITrack() = default;

    virtual std::optional<std::string> get_artist() const = 0;
    virtual std::optional<std::string> get_albumName() const = 0;
    virtual std::optional<std::string> get_albumArtist() const = 0;
    virtual std::optional<std::string> get_copyright() const = 0;
    virtual std::optional<std::string> get_genre() const = 0;
    virtual std::optional<size_t> get_year() const = 0;
    virtual std::optional<std::string> get_label() const = 0;
    virtual std::optional<size_t> get_trackNumber() const = 0;
    virtual std::optional<size_t> get_discNumber() const = 0;
    virtual std::size_t get_length() const = 0;

    virtual bool is_verified() const = 0;
    virtual void set_verified(bool _verified) = 0;
};
