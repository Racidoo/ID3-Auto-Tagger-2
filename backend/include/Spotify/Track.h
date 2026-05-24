#pragma once

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
#include "Interfaces/ITrack.h"

namespace Spotify {
class Track : public ITrack {
  public:
    Track(const std::string &_id, const std::optional<std::string> &_name,
          State _state, std::optional<std::size_t> _discNumber,
          std::size_t _length, bool _explicitContent,
          std::optional<std::size_t> _trackNumber, std::weak_ptr<Album> _album,
          const std::vector<std::shared_ptr<IArtist>> &_artists,
          IMediaService *_mediaService);
    ~Track() = default;

    std::optional<std::string> get_name() const override;
    std::vector<std::shared_ptr<IArtist>> get_artists() const;
    std::weak_ptr<IAlbum> get_album();
    std::optional<std::string> get_artist() const override;
    std::optional<size_t> get_trackNumber() const override;
    std::optional<size_t> get_discNumber() const override;
    std::size_t get_length() const override;
    std::optional<std::string> get_albumName() const override;
    std::optional<std::string> get_albumArtist() const override;
    std::optional<std::string> get_copyright() const override;
    std::optional<std::string> get_genre() const override;
    std::optional<size_t> get_year() const override;
    std::optional<std::string> get_label() const override;
    std::optional<std::vector<std::byte>> get_image() override;

    bool is_verified() const override;
    void set_verified(bool _verified) override;

  private:
    std::optional<std::string> name;
    std::optional<std::size_t> discNumber;
    std::size_t length;
    bool explicitContent;
    std::optional<std::size_t> trackNumber;
    std::weak_ptr<Album> album;
    std::vector<std::shared_ptr<IArtist>> artists;
    std::optional<std::string> genre;

    bool downloaded;
};

} // namespace Spotify
