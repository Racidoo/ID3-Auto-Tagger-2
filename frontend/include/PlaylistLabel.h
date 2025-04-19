#if !defined(PLAYLIST_LABEL_H)
#define PLAYLIST_LABEL_H

#include <memory>
#include <wx/wx.h>

#include "MediaLabel.h"
#include "Spotify/Playlist.h"

class PlaylistLabel : public MediaLabel {
  private:
    std::unique_ptr<Spotify::Playlist> spotifyPlaylist;

  public:
    PlaylistLabel(wxWindow *_parent, const Spotify::Playlist &_playlist);

    inline const Spotify::Playlist *get_spotifyPlaylist() const {
        return spotifyPlaylist.get();
    }
    inline const Spotify::Playlist *getSpotifyObject() const override {
        return spotifyPlaylist.get();
    }
};

#endif // PLAYLIST_LABEL_H
