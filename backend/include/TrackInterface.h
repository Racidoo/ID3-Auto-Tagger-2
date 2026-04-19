#if !defined(TRACK_INTERFACE_H)
#define TRACK_INTERFACE_H

#include <functional>
#include <memory>
#include <string>

#include "LocalTrack.h"
#include "Spotify/Track.h"

class Downloader;

class TrackInterface {
  private:
    bool inBlocklist = false;
    bool verified = false;
    bool downloaded = false;

    std::shared_ptr<LocalTrack> local;
    std::shared_ptr<Spotify::Track> spotify;

  public:
    TrackInterface() = default;
    ~TrackInterface() = default;

    std::string get_id() const;
    std::string get_title();
    std::string get_artist();
    std::string get_album();
    std::string get_albumArtist();
    std::string get_copyright();
    std::string get_genre();
    std::string get_year();
    std::string get_label();
    std::string get_track();
    std::string get_disc();
    std::size_t get_length();

    std::vector<std::byte> get_cover();

    const std::shared_ptr<LocalTrack> get_localTrack() const;
    const std::shared_ptr<Spotify::Track> get_spotifyTrack() const;

    bool is_inBlocklist() const;
    bool is_verified() const;
    bool is_downloaded() const;
    bool is_localTrack() const;
    bool is_spotifyTrack() const;

    void set_title(const std::string &_title);
    void set_artist(const std::string &_artist);
    void set_album(const std::string &_album);
    void set_albumArtist(const std::string &_albumArtist);
    void set_copyright(const std::string &_copyright);
    void set_genre(const std::string &_genre);
    void set_year(const std::string &_year);
    void set_label(const std::string &_label);
    void set_track(const std::string &_track);
    void set_disc(const std::string &_disc);

    void set_cover(const std::vector<std::byte> &_imageData);

    void set_inBlocklist(bool _inBlocklist);
    void set_verified(bool _verified);
    void set_downloaded(bool _downloaded);

    void verifyTags(std::shared_ptr<TrackInterface> _template);

    static std::shared_ptr<TrackInterface> fromLocal(const LocalTrack &_track);
    static std::shared_ptr<TrackInterface>
    fromSpotify(const Spotify::Track &_track);

    static void verify(std::shared_ptr<TrackInterface> _data,
                       Downloader *_downloader);
};

#endif // TRACK_INTERFACE_H
