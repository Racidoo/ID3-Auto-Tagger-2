#pragma once

#include "Discogs/Label.h"
#include "Interfaces/IRemoteImageProvider.h"
#include "Interfaces/ITrack.h"

class IArtist;
class IAlbum;

namespace Discogs {
class Release;
class ReleaseTrack : public ITrack {
  public:
    ReleaseTrack(const std::string &_id, const std::string &_name,
                 std::weak_ptr<Release> _release,
                 const std::vector<std::shared_ptr<IArtist>> &_artists,
                 std::size_t _duration, const std::size_t &_discnumber,
                 const std::size_t &_tracknumber, IMediaService *_mediaService);
    ~ReleaseTrack() = default;

    const std::string &get_name() const override;
    std::string get_artist() const override;
    const std::string &get_albumName() const override;
    std::string get_albumArtist() const override;
    const std::string &get_copyright() const override;
    const std::string &get_genre() const override;
    std::size_t get_year() const override;
    const std::string &get_label() const override;
    std::size_t get_trackNumber() const override;
    std::size_t get_discNumber() const override;
    std::size_t get_length() const override;
    std::vector<std::byte> get_image() override;
    bool is_verified() const override;

    void set_name(const std::string &_name) override;
    void set_artist(const std::string &_artist) override;
    void set_albumName(const std::string &_albumName) override;
    void set_albumArtist(const std::string &_albumArtist) override;
    void set_copyright(const std::string &_copyright) override;
    void set_genre(const std::string &_genre) override;
    void set_year(std::size_t _year) override;
    void set_label(const std::string &_label) override;
    void set_trackNumber(std::size_t _trackNumber) override;
    void set_discNumber(std::size_t _discNumber) override;
    void set_verified(bool _verified) override;
    void set_image(const std::vector<std::byte> &_imageData) override;

    std::weak_ptr<IAlbum> get_release() const;
    const std::vector<std::shared_ptr<IArtist>> &get_artists() const;

    static int parseDuration(const std::string &_stringDuration);

  private:
    IRemoteImageProvider imageProvider;
    std::string name;
    std::weak_ptr<Release> release;
    std::vector<std::shared_ptr<IArtist>> artists;
    std::size_t duration;
    std::size_t discnumber;
    std::size_t tracknumber;
    bool verified;
};

} // namespace Discogs