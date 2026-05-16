#pragma once

#include "Interfaces/IAlbum.h"

class ITrack;
class IArtist;

namespace Spotify {

class Album : public IAlbum {
  public:
    Album(const std::string &_id, const std::string &_name, State _state,
          const std::string &_albumType, std::size_t _totalTracks,
          const std::string &_releaseDate, std::size_t _year,
          const std::string &_imageURL,
          const std::vector<std::shared_ptr<IArtist>> &_artists);
    ~Album() = default;

    const std::string &get_type() const override;
    std::string get_artist() const override;
    const std::vector<std::byte> &get_artistImage();
    std::size_t get_year() const override;
    const std::string &get_label() const override;
    const std::string &get_copyright() const override;
    const std::vector<std::shared_ptr<ITrack>> &get_tracklist() const override;

    const std::string get_releaseDate() const;
    const std::vector<std::shared_ptr<IArtist>> &get_artists() const;
    std::vector<std::shared_ptr<IArtist>> &get_artists();
    std::size_t get_totalTracks() const;

    void set_label(const std::string &_label) override;
    void set_copyright(const std::string &_copyright) override;
    void set_tracklist(
        const std::vector<std::shared_ptr<ITrack>> &_tracklist) override;
    void set_year(std::size_t _year) override;

  private:
    std::string type;
    size_t year;
    std::size_t totalTracks;
    std::string releaseDate;
    std::vector<std::shared_ptr<IArtist>> artists;

    // optional attributes
    std::string label;
    std::string copyright;
    std::vector<std::shared_ptr<ITrack>> tracklist;
};

} // namespace Spotify