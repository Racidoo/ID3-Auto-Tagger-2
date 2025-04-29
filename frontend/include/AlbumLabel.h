#if !defined(ALBUM_LABEL_H)
#define ALBUM_LABEL_H

#include <memory>
#include <wx/wx.h>

#include "MediaLabel.h"
#include "Spotify/Album.h"
class AlbumLabel : public MediaLabel {
  private:
    std::unique_ptr<Spotify::Album> spotifyAlbum;

  public:
    AlbumLabel(wxWindow *_parent, const Spotify::Album &_album);

    inline const Spotify::Album *get_spotifyAlbum() const {
        return spotifyAlbum.get();
    }
    inline const Spotify::SpotifyObject *getSpotifyObject() const override {
        return spotifyAlbum.get();
    }
};
#endif // ALBUM_LABEL_H
