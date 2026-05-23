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

    Release(int _id, const std::string &_name, State _state,
            const std::string &_imageUrl,
            const std::vector<std::shared_ptr<IArtist>> &_artists,
            int _masterId, IAlbum::album_type_t _type,
            const std::vector<std::string> &_genres,
            const std::vector<Label> &_labels, const std::string &_copyright,
            std::size_t _year, const std::vector<Video> &_videos,
            bool _verified, IMediaService *_mediaService);
    ~Release() = default;

    const std::string &get_name() const override;
    IAlbum::album_type_t get_type() const override;
    std::string get_artist() const override;
    std::vector<std::byte> get_artistImage() override;
    std::size_t get_year() const override;
    const std::string &get_label() const override;
    const std::string &get_copyright() const override;
    const std::vector<std::shared_ptr<ITrack>> &get_tracklist() const override;
    std::vector<std::byte> get_image() override;

    const std::vector<std::shared_ptr<IArtist>> &get_artists() const;
    std::vector<std::shared_ptr<IArtist>> &get_artists();

    const std::vector<std::string> &get_genres() const;
    const std::string &get_stringGenres() const;
    const std::vector<Video> &get_videos() const;
    int get_masterId() const;
    const std::vector<Label> &get_labels() const;
    std::string get_stringLabels() const;
    const std::string &get_releaseDate() const;
    bool isVerified() const;

    const IRemoteImageProvider &get_imageProvider() const;

    void set_year(std::size_t _year) override;
    void set_copyright(const std::string &_copyright) override;
    void set_label(const std::string &_label) override;
    void set_tracklist(
        const std::vector<std::shared_ptr<ITrack>> &_tracklist) override;

    void set_artists(std::vector<std::shared_ptr<IArtist>> _artists);

  private:
    std::string name;
    IRemoteImageProvider imageProvider;
    std::vector<std::shared_ptr<IArtist>> artists;
    std::string stringGenres;
    std::vector<std::string> genres;
    std::vector<std::shared_ptr<ITrack>> tracklist;
    int year;
    std::vector<Video> videos;
    int masterId;
    IAlbum::album_type_t type;
    std::vector<Label> labels;
    std::string stringLabels;
    std::string copyright;
    bool verified;
};

} // namespace Discogs
