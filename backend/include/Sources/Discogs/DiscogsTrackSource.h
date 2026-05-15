#pragma once

#include <memory>

#include "Discogs/ReleaseTrack.h"
#include "Sources/ITrackSource.hpp"

class DiscogsTrackSource : public ITrackSource {
  public:
    explicit DiscogsTrackSource(std::shared_ptr<Discogs::ReleaseTrack> _track)
        : track(_track) {}

    const std::string &get_id() const override;
    const std::string &get_name() override;
    std::string get_artist() override;
    std::string get_album() override;
    std::string get_albumArtist() override;
    std::string get_copyright() override;
    std::string get_genre() override; // not supported
    std::string get_year() override;
    std::string get_label() override;
    std::string get_trackNumber() override;
    std::string get_discNumber() override;
    std::size_t get_length() override;
    const std::vector<std::byte> &get_image() override;
    IMediaEntity::State get_state() const override;

    // Discogs object should only be mutated by DiscogsAPI
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
    void set_state(IMediaEntity::State _state) override;
    std::shared_ptr<Discogs::ReleaseTrack> get_track() const;

    void ensureLoaded(class IMediaService &_service) override;

  private:
    std::shared_ptr<Discogs::ReleaseTrack> track;
};