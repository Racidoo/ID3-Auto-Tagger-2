#pragma once

#include "Interfaces/IAlbum.h"
#include "Interfaces/IRemoteImageProvider.h"

class ITrack;
class IArtist;

namespace Spotify {
class Album : public IAlbum {
  public:
    Album(const std::string &_id, const std::string &_name, State _state,
          IAlbum::album_type_t _albumType, std::size_t _totalTracks,
          const std::string &_releaseDate, std::optional<std::size_t> _year,
          const std::string &_imageURL,
          const std::vector<std::shared_ptr<IArtist>> &_artists,
          IMediaService *_mediaService);
    ~Album() = default;

    std::optional<std::string> get_name() const override;
    std::optional<std::vector<std::byte>> get_image() override;
    IAlbum::album_type_t get_type() const override;
    std::optional<std::string> get_artist() const override;
    std::optional<std::vector<std::byte>> get_artistImage() override;
    std::optional<std::size_t> get_year() const override;
    std::optional<std::string> get_label() const override;
    std::optional<std::string> get_copyright() const override;
    const std::vector<std::shared_ptr<ITrack>> &get_tracklist() const override;

    const std::string get_releaseDate() const;
    const std::vector<std::shared_ptr<IArtist>> &get_artists() const;
    std::vector<std::shared_ptr<IArtist>> &get_artists();
    std::size_t get_totalTracks() const;
    const IRemoteImageProvider &get_imageProvider() const;

    void set_label(const std::optional<std::string> &_label);
    void set_copyright(const std::optional<std::string> &_copyright);
    void set_tracklist(const std::vector<std::shared_ptr<ITrack>> &_tracklist);

  private:
    std::optional<std::string> name;
    IRemoteImageProvider imageProvider;
    IAlbum::album_type_t type;
    std::optional<size_t> year;
    std::size_t totalTracks;
    std::string releaseDate;
    std::vector<std::shared_ptr<IArtist>> artists;

    // optional attributes
    std::optional<std::string> label;
    std::optional<std::string> copyright;
    std::vector<std::shared_ptr<ITrack>> tracklist;
};
} // namespace Spotify