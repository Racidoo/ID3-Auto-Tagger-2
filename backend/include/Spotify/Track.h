#if !defined(SPOTIFY_TRACK_H)
#define SPOTIFY_TRACK_H

#include <fstream>
#include <iostream>
#include <string>
#include <taglib/apetag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>
#include <vector>

#include "Album.h"
#include "SpotifyObject.h"

namespace Spotify {

class Track : public SpotifyObject {
  private:
    const unsigned int discNumber;
    const unsigned long durationMs;
    const bool explicitContent;
    const unsigned int trackNumber;
    const Album album;
    std::vector<Artist> artists;

    bool downloaded;

  public:
    Track(const std::string &_id, const std::string &_name,
          const unsigned int _discNumber, const unsigned long _durationMs,
          const bool _explicitContent, const unsigned int _trackNumber,
          const Album &_album, const std::vector<Artist> &_artists);
    ~Track();

    inline const std::vector<Artist> get_artists() const { return artists; }
    inline const Album get_album() const { return album; }
    inline const std::string get_stringArtists() const {
        return Artist::vecToStr(artists);
    }
    inline unsigned int get_durationMs() const { return durationMs; }
    inline bool isDownloaded() const { return downloaded; }

    inline void set_downloaded(bool _downloaded) { downloaded = _downloaded; }

    bool writeID3V2Tags(TagLib::MPEG::File _file) const;
    bool setAlbumCover(const std::string &_mp3Path,
                       const std::string &_imagePath) const;
    bool setAlbumCover(const std::string &_mp3Path,
                       const std::vector<char> &_imageData) const;
    static void printID3V2Tags(TagLib::MPEG::File _file);

  private:
    bool setTagValue(TagLib::MPEG::File &file, const char *frameID,
                     const std::string &value) const;
    bool setAPICTag(const std::string &_mp3Path,
                    const std::vector<char> &_imageData) const;
};

} // namespace Spotify
#endif // SPOTIFY_TRACK_H
