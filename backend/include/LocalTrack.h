#if !defined(LOCAL_TRACK_H)
#define LOCAL_TRACK_H

#include <filesystem>
#include <string>
#include <taglib/apetag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>

class LocalTrack {
  private:
    std::filesystem::path filepath;

    bool tagsLoaded;
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    std::string year;
    std::string trackNumber;
    std::string albumArtist;
    std::string copyright;
    std::string label;
    std::string discNumber;
    int length;

  public:
    LocalTrack(const std::filesystem::path &_path);
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
        DISC
    };

    std::string get_title();
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

    std::vector<std::byte> get_cover() const;

    const std::filesystem::path &get_filepath() const;
    std::string get_filename() const;

    void set_title(const std::string &_title);
    void set_artist(const std::string &_artist);
    void set_album(const std::string &_album);
    void set_albumArtist(const std::string &_albumArtist);
    void set_copyright(const std::string &_copyright);
    void set_genre(const std::string &_genre);
    void set_year(const std::string &_year);
    void set_label(const std::string &_label);
    void set_trackNumber(const std::string &_trackNumber);
    void set_discNumber(const std::string &_discNumber);
    void set_cover(const std::vector<std::byte> &_imageData);

    void set_filepath(const std::filesystem::path &_filepath);

    void ensureTagsLoaded();

    static std::string getFrameText(TagLib::ID3v2::Tag *_tag, const char *_id);
    static bool setTagValue(const std::filesystem::path &_filepath,
                            const char *frameID, const std::string &value);
};

#endif // LOCAL_TRACK_H
