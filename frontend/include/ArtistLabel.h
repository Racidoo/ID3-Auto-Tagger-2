#if !defined(ARTISTLABEL_H)
#define ARTISTLABEL_H

#include <memory>
#include <wx/wx.h>

#include "MediaLabel.h"
#include "Spotify/Artist.h"

class ArtistLabel : public MediaLabel {
  private:
    std::unique_ptr<Spotify::Artist> spotifyArtist;

  public:
    ArtistLabel(wxWindow *_parent, const Spotify::Artist &_artist);

    inline const Spotify::Artist *get_spotifyArtist() const {
        return spotifyArtist.get();
    }
    inline const Spotify::Artist *getSpotifyObject() const override {
        return spotifyArtist.get();
    }
};

#endif // ARTISTLABEL_H
