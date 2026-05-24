#pragma once

#include "Discogs/Label.h"
#include "Interfaces/IAlbum.h"
#include "Interfaces/IRemoteImageProvider.h"

class IArtist;
namespace Discogs {
class Release : public IAlbum {
  public:
    struct Video {
        std::string description;
        std::string title;
        std::string uri;
        int duration;
    };

    Release(int _id, const std::optional<std::string> &_name, State _state,
            const std::string &_imageUrl,
            const std::vector<std::shared_ptr<IArtist>> &_artists,
            int _masterId, IAlbum::album_type_t _type,
            const std::vector<std::string> &_genres,
            const std::vector<Label> &_labels, const std::string &_copyright,
            std::optional<std::size_t> _year, const std::vector<Video> &_videos,
            bool _verified, IMediaService *_mediaService);
    ~Release() = default;

    std::optional<std::string> get_name() const override;
    IAlbum::album_type_t get_type() const override;
    std::optional<std::string> get_artist() const override;
    std::optional<std::vector<std::byte>> get_artistImage() override;
    std::optional<std::size_t> get_year() const override;
    std::optional<std::string> get_label() const override;
    std::optional<std::string> get_copyright() const override;
    const std::vector<std::shared_ptr<ITrack>> &get_tracklist() const override;
    std::optional<std::vector<std::byte>> get_image() override;

    const std::vector<std::shared_ptr<IArtist>> &get_artists() const;
    std::vector<std::shared_ptr<IArtist>> &get_artists();

    const std::vector<std::string> &get_genres() const;
    std::optional<std::string> get_stringGenres() const;
    const std::vector<Video> &get_videos() const;
    int get_masterId() const;
    const std::vector<Label> &get_labels() const;
    bool isVerified() const;

    const IRemoteImageProvider &get_imageProvider() const;

    void set_year(std::size_t _year);
    void set_copyright(const std::string &_copyright);
    void set_label(const std::string &_label);
    void set_tracklist(const std::vector<std::shared_ptr<ITrack>> &_tracklist);
    void set_artists(std::vector<std::shared_ptr<IArtist>> _artists);

  private:
    std::optional<std::string> name;
    IRemoteImageProvider imageProvider;
    std::vector<std::shared_ptr<IArtist>> artists;
    std::vector<std::string> genres;
    std::vector<std::shared_ptr<ITrack>> tracklist;
    std::optional<std::size_t> year;
    std::vector<Video> videos;
    int masterId;
    IAlbum::album_type_t type;
    std::vector<Label> labels;
    std::optional<std::string> copyright;
    bool verified;
};

} // namespace Discogs
