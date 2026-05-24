#pragma once

#include "Discogs/Label.h"
#include "Interfaces/ITrack.h"

class IArtist;
class IAlbum;

namespace Discogs {
class Release;
class ReleaseTrack : public ITrack {
  public:
    ReleaseTrack(const std::string &_id,
                 const std::optional<std::string> &_name,
                 std::weak_ptr<Release> _release,
                 const std::vector<std::shared_ptr<IArtist>> &_artists,
                 std::size_t _duration,
                 const std::optional<std::size_t> &_discnumber,
                 const std::optional<std::size_t> &_tracknumber,
                 IMediaService *_mediaService);
    ~ReleaseTrack() = default;

    std::optional<std::string> get_name() const override;
    std::optional<std::string> get_artist() const override;
    std::optional<std::string> get_albumName() const override;
    std::optional<std::string> get_albumArtist() const override;
    std::optional<std::string> get_copyright() const override;
    std::optional<std::string> get_genre() const override;
    std::optional<size_t> get_year() const override;
    std::optional<std::string> get_label() const override;
    std::optional<size_t> get_trackNumber() const override;
    std::optional<size_t> get_discNumber() const override;
    std::size_t get_length() const override;
    std::optional<std::vector<std::byte>> get_image() override;

    std::weak_ptr<IAlbum> get_release() const;
    const std::vector<std::shared_ptr<IArtist>> &get_artists() const;

    bool is_verified() const override;
    void set_verified(bool _verified) override;

    static int parseDuration(const std::string &_stringDuration);

  private:
    std::optional<std::string> name;
    std::weak_ptr<Release> release;
    std::vector<std::shared_ptr<IArtist>> artists;
    std::size_t duration;
    std::optional<std::size_t> discnumber;
    std::optional<std::size_t> tracknumber;
    bool verified;
};

} // namespace Discogs