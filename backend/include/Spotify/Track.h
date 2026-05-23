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
#include "Interfaces/IRemoteImageProvider.h"
#include "Interfaces/ITrack.h"

namespace Spotify {

class Track : public ITrack {
  public:
    Track(const std::string &_id, const std::string &_name, State _state,
          std::size_t _discNumber, std::size_t _length, bool _explicitContent,
          std::size_t _trackNumber, std::shared_ptr<Album> _album,
          const std::vector<std::shared_ptr<IArtist>> &_artists,
          IMediaService *_mediaService);
    ~Track() = default;

    const std::string &get_name() const override;
    const std::vector<std::shared_ptr<IArtist>> &get_artists();
    std::shared_ptr<IAlbum> get_album();
    std::string get_artist() const override;
    std::size_t get_trackNumber() const override;
    std::size_t get_discNumber() const override;
    std::size_t get_length() const override;
    const std::string &get_albumName() const override;
    std::string get_albumArtist() const override;
    const std::string &get_copyright() const override;
    const std::string &get_genre() const override;
    std::size_t get_year() const override;
    const std::string &get_label() const override;
    bool is_verified() const override;
    std::vector<std::byte> get_image() override;

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
    void set_verified(bool _verified) override;
    void set_image(const std::vector<std::byte> &_imageData) override;

  private:
    std::string name;
    IRemoteImageProvider imageProvider;
    std::size_t discNumber;
    unsigned long length;
    bool explicitContent;
    std::size_t trackNumber;
    std::shared_ptr<Album> album;
    std::vector<std::shared_ptr<IArtist>> artists;
    std::string genre;

    bool downloaded;
};

} // namespace Spotify
