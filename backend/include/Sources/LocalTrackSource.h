#pragma once

#include "ITrackSource.hpp"
#include "LocalTrack.h"

class LocalTrackSource : public ITrackSource {
  private:
    std::shared_ptr<LocalTrack> track;

  public:
    explicit LocalTrackSource(std::shared_ptr<LocalTrack> _track)
        : track(std::move(_track)) {}

    const std::string &get_id() const override;
    const std::string &get_name() override;
    const std::vector<std::byte> &get_image() override;
    std::string get_artist() override;
    std::string get_album() override;
    std::string get_albumArtist() override;
    std::string get_copyright() override;
    std::string get_genre() override;
    std::string get_year() override;
    std::string get_label() override;
    std::string get_trackNumber() override;
    std::string get_discNumber() override;
    std::size_t get_length() override;
    IMediaEntity::State get_state() const;

    void set_name(const std::string &_name) override;
    void set_artist(const std::string &_artist) override;
    void set_album(const std::string &_album) override;
    void set_albumArtist(const std::string &_albumArtist) override;
    void set_copyright(const std::string &_copyright) override;
    void set_genre(const std::string &_genre) override;
    void set_year(const std::string &_year) override;
    void set_label(const std::string &_label) override;
    void set_trackNumber(const std::string &_track) override;
    void set_discNumber(const std::string &_disc) override;
    void set_image(const std::vector<std::byte> &_imageData) override;
    void set_state(IMediaEntity::State _state);

    std::shared_ptr<LocalTrack> get_track() const;

    void ensureLoaded(class IMediaService &_service) override;
};