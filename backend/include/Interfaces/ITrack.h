#pragma once

#include <iostream>
#include <memory>

#include "MediaEntityBase.h"

class ITrack : public MediaEntityBase {

  public:
    explicit ITrack(const std::string &_id, const std::string &_name,
                    State _state, const std::string &_imageURL,
                    IMediaService *_mediaService)
        : MediaEntityBase(_id, _name, _state, _imageURL, _mediaService) {}
    ~ITrack() = default;

    virtual std::string get_artist() const = 0;
    virtual const std::string &get_albumName() const = 0;
    virtual std::string get_albumArtist() const = 0;
    virtual const std::string &get_copyright() const = 0;
    virtual const std::string &get_genre() const = 0;
    virtual std::size_t get_year() const = 0;
    virtual const std::string &get_label() const = 0;
    virtual std::size_t get_trackNumber() const = 0;
    virtual std::size_t get_discNumber() const = 0;
    virtual std::size_t get_length() const = 0;

    virtual bool is_verified() const = 0;

    virtual void set_artist(const std::string &_artist) = 0;
    virtual void set_albumName(const std::string &_albumName) = 0;
    virtual void set_albumArtist(const std::string &_albumArtist) = 0;
    virtual void set_copyright(const std::string &_copyright) = 0;
    virtual void set_genre(const std::string &_genre) = 0;
    virtual void set_year(std::size_t _year) = 0;
    virtual void set_label(const std::string &_label) = 0;
    virtual void set_trackNumber(std::size_t _trackNumber) = 0;
    virtual void set_discNumber(std::size_t _discNumber) = 0;

    virtual void set_verified(bool _verified) = 0;

    void applyDifferences(std::shared_ptr<ITrack> _template);
};
