#if !defined(LOCAL_TRACK_H)
#define LOCAL_TRACK_H

#include <filesystem>
#include <string>

#include "Interfaces/IFileImageProvider.h"
#include "Interfaces/ITrack.h"

class LocalTrack : public ITrack {
  private:
    std::filesystem::path filepath;
    IFileImageProvider imageProvider;
    bool verified;

    // preview tags
    std::string name;
    std::string artist;
    std::string album;
    std::string genre;
    std::size_t year;
    std::size_t trackNumber;
    int length;

    // full tags
    std::string albumArtist;
    std::string copyright;
    std::string label;
    std::size_t discNumber;
    std::size_t bitrate;
    std::size_t sampleRate;
    std::size_t channels;

  public:
    LocalTrack(const std::filesystem::path &_path,
               IMediaService *_mediaService);
    ~LocalTrack();

    enum tag_type_t {
        TITLE = 0,
        ARTIST,
        ALBUM,
        ALBUM_ARTIST,
        GENRE,
        LABEL,
        COPYRIGHT,
        YEAR,
        TRACK,
        DISC,
        FILENAME,
        FILE_EXTENSION,
        BITRATE,
        CHANNELS,
        SAMPLE_RATE
    };

    const std::string &get_name() const override;
    std::string get_artist() const override;
    const std::string &get_albumName() const override;
    const std::string &get_genre() const override;
    std::size_t get_year() const override;
    std::size_t get_trackNumber() const override;
    std::size_t get_length() const override;
    std::string get_albumArtist() const override;
    const std::string &get_copyright() const override;
    const std::string &get_label() const override;
    std::size_t get_discNumber() const override;
    std::string get_bitrate() const;
    std::string get_sampleRate() const;
    std::string get_channels() const;
    std::vector<std::byte> get_image() override;
    bool is_verified() const override;

    const std::filesystem::path &get_filepath() const;

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
    void set_image(const std::vector<std::byte> &_imageData) override;
    void set_verified(bool _verified) override;

    void set_filepath(const std::filesystem::path &_filepath);

    void ensurePreviewTagsLoaded();
    void ensureFullTagsLoaded();

    static std::string getFrameText(TagLib::ID3v2::Tag *_tag, const char *_id);
    static bool setTagValue(const std::filesystem::path &_filepath,
                            const char *frameID, const std::string &value);
};

#endif // LOCAL_TRACK_H
