#pragma once

#include "Discogs/Label.h"
#include "Interfaces/ITrack.h"

class IArtist;
class IAlbum;

namespace Discogs {

class ReleaseTrack : public ITrack {
  public:
    ReleaseTrack(const std::string &_id, const std::string &_name,
                 std::weak_ptr<IAlbum> _release,
                 const std::vector<std::shared_ptr<IArtist>> &_artists,
                 const std::vector<std::shared_ptr<IArtist>> &_extraArtists,
                 const std::vector<std::string> &_genres,
                 const std::string &_imageUrl, std::size_t _duration,
                 const std::string &_position);
    ~ReleaseTrack() = default;

    std::string get_artist() const override;
    const std::string &get_albumName() const override;
    std::string get_albumArtist() override;
    const std::string &get_copyright() override;
    const std::string &get_genre() const override;
    std::size_t get_year() const override;
    const std::string &get_label() override;
    std::size_t get_trackNumber() const override;
    std::size_t get_discNumber() override;
    std::size_t get_length() const override;

    bool is_verified() const override;
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

    std::weak_ptr<IAlbum> get_release() const;
    const std::vector<std::shared_ptr<IArtist>> &get_artists() const;
    const std::vector<std::shared_ptr<IArtist>> &get_extraArtists() const;

    static int parseDuration(const std::string &_stringDuration);

  private:
    std::weak_ptr<IAlbum> release;
    std::vector<std::shared_ptr<IArtist>> artists;
    std::vector<std::shared_ptr<IArtist>> extraArtists;
    std::string genres;
    std::size_t trackNumber;
    std::size_t duration;
    std::string position;
    bool verified;
};

} // namespace Discogs