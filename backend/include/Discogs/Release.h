#pragma once

#include "Discogs/Label.h"
#include "Interfaces/IAlbum.h"

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
            int _masterId, const std::string &_country,
            const std::vector<std::string> &_genres,
            const std::vector<Label> &_labels, const std::string &_copyright,
            const std::string &_releaseDate,
            const std::vector<std::string> &_styles, std::size_t _year,
            const std::vector<Video> &_videos, bool _verified);
    ~Release() = default;

    const std::string &get_type() const override;
    std::string get_artist() const override;
    const std::vector<std::byte> &get_artistImage() override;
    std::size_t get_year() const override;
    const std::string &get_label() const override;
    const std::string &get_copyright() const override;
    const std::vector<std::shared_ptr<ITrack>> &get_tracklist() const override;

    void set_year(std::size_t _year) override;
    void set_copyright(const std::string &_copyright) override;
    void set_label(const std::string &_label) override;
    void set_tracklist(
        const std::vector<std::shared_ptr<ITrack>> &_tracklist) override;

    const std::vector<std::shared_ptr<IArtist>> &get_artists() const;
    std::vector<std::shared_ptr<IArtist>> &get_artists();

    const std::vector<std::string> &get_genres() const;
    std::string get_stringGenres() const;
    const std::vector<std::string> &get_styles() const;
    std::string get_stringStyles() const;
    const std::vector<Video> &get_videos() const;
    int get_masterId() const;
    const std::string &get_country() const;
    const std::vector<Label> &get_labels() const;
    std::string get_stringLabels() const;
    const std::string &get_releaseDate() const;
    bool isVerified() const;

  private:
    std::vector<std::shared_ptr<IArtist>> artists;
    std::vector<std::string> genres;
    std::vector<std::string> styles;
    std::vector<std::shared_ptr<ITrack>> tracklist;
    int year;
    std::vector<Video> videos;
    int masterId;
    std::string country;
    std::vector<Label> labels;
    std::string copyright;
    std::string releaseDate;
    std::string type;
    bool verified;
};

} // namespace Discogs
