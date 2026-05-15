#pragma once

#include <functional>
#include <memory>

#include "IMediaEntity.hpp"

class Downloader;
class LocalTrack;
class ITrackSource;

namespace Discogs {
class ReleaseTrack;
} // namespace Discogs

namespace Spotify {
class Track;
} // namespace Spotify

class ITrack : public IMediaEntity {

  public:
    explicit ITrack(std::shared_ptr<ITrackSource> _source)
        : source(std::move(_source)) {}
    ~ITrack() = default;

    const std::string &get_id() const override;
    const std::string &get_name() const override;
    const std::vector<std::byte> &get_image() override;
    std::string get_artist();
    std::string get_album();
    std::string get_albumArtist();
    std::string get_copyright();
    std::string get_genre();
    std::string get_year();
    std::string get_label();
    std::string get_trackNumber();
    std::string get_discNumber();
    std::size_t get_length();

    bool is_verified() const;

    void set_name(const std::string &_name);
    void set_artist(const std::string &_artist);
    void set_album(const std::string &_album);
    void set_albumArtist(const std::string &_albumArtist);
    void set_copyright(const std::string &_copyright);
    void set_genre(const std::string &_genre);
    void set_year(const std::string &_year);
    void set_label(const std::string &_label);
    void set_trackNumber(const std::string &_track);
    void set_discNumber(const std::string &_disc);
    void set_image(const std::vector<std::byte> &_imageData);

    void set_verified(bool _verified);

    void ensureLoaded(class IMediaService &_service) override;

    std::shared_ptr<Spotify::Track> get_spotifyTrack() const;
    std::shared_ptr<LocalTrack> get_localTrack() const;
    // std::shared_ptr<Discogs::ReleaseTrack> get_discogsTrack() const;

    void verifyTags(std::shared_ptr<Spotify::Track> _template);

    static std::shared_ptr<ITrack>
    fromLocal(std::shared_ptr<LocalTrack> _track);
    static std::shared_ptr<ITrack>
    fromSpotify(std::shared_ptr<Spotify::Track> _track);
    static std::shared_ptr<ITrack>
    fromDiscogs(std::shared_ptr<Discogs::ReleaseTrack> _track);

    static bool verify(std::shared_ptr<ITrack> _data, Downloader *_downloader);

  private:
    bool verified = false;
    std::shared_ptr<ITrackSource> source;
};
