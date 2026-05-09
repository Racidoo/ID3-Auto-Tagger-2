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
#include "QueryObject.h"

namespace Spotify {

class Track : public QueryObject {
  public:
    Track(const std::string &_id, const std::string &_name,
          unsigned int _discNumber, unsigned long _durationMs,
          bool _explicitContent, unsigned int _trackNumber, const Album &_album,
          const std::vector<Artist> &_artists);
    ~Track() = default;

    const std::string get_imageUrl() = delete;
    const std::vector<Artist> &get_artists() const;
    std::vector<Artist> &get_artists();
    Album &get_album();
    const Album &get_album() const;
    std::string get_stringArtists() const;
    unsigned int get_trackNumber() const;
    unsigned int get_discNumber() const;
    unsigned int get_durationMs() const;
    const std::string &get_genre() const;
    bool isDownloaded() const;

    void set_genre(const std::string &_genre);
    void set_downloaded(bool _downloaded);

  private:
    unsigned int discNumber;
    unsigned long durationMs;
    bool explicitContent;
    unsigned int trackNumber;
    Album album;
    std::vector<Artist> artists;
    std::string genre;

    bool downloaded;
};

} // namespace Spotify
#endif // SPOTIFY_TRACK_H
