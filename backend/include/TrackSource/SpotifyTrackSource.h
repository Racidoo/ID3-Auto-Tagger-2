#pragma once

#include "ITrackSource.hpp"
#include "Spotify/Track.h"

class SpotifyTrackSource : public ITrackSource {
  private:
    std::shared_ptr<Spotify::Track> track;

  public:
    explicit SpotifyTrackSource(std::shared_ptr<Spotify::Track> _track)
        : track(std::move(_track)) {}

    std::string get_id() const override;
    std::string get_title() override;
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
    std::vector<std::byte> get_cover() override;

    // Spotify object should only be mutated by SpotifyAPI
    void set_title(const std::string &_title) override;
    void set_artist(const std::string &_artist) override;
    void set_album(const std::string &_album) override;
    void set_albumArtist(const std::string &_albumArtist) override;
    void set_copyright(const std::string &_copyright) override;
    void set_genre(const std::string &_genre) override;
    void set_year(const std::string &_year) override;
    void set_label(const std::string &_label) override;
    void set_trackNumber(const std::string &_track) override;
    void set_discNumber(const std::string &_disc) override;
    void set_cover(const std::vector<std::byte> &_imageData) override;

    std::shared_ptr<Spotify::Track> get_track() const;
};