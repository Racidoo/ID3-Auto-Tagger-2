#if !defined(SPOTIFY_WINDOW_H)
#define SPOTIFY_WINDOW_H

#include <vector>
#include <wx/mdi.h>
#include <wx/tglbtn.h>
#include <wx/wx.h>

// #include "AlbumLabel.h"
// #include "ArtistLabel.h"
#include "Downloader.h"
#include "MediaWindow.hpp"
// #include "PlaylistLabel.h"
// #include "Spotify/Album.h"
// #include "Spotify/Artist.h"
// #include "Spotify/Playlist.h"
// #include "Spotify/Track.h"
#include "TrackWindow.h"

// class Downloader;
class AlbumLabel;
class ArtistLabel;
class PlaylistLabel;
class TrackWindow;

using AlbumWindow = MediaWindow<AlbumLabel>;
using ArtistWindow = MediaWindow<ArtistLabel>;
using PlaylistWindow = MediaWindow<PlaylistLabel>;

class SpotifyWindow : public wxScrolledWindow {
  private:
    Downloader *downloader;

    wxStaticText *trackHeader;
    wxStaticText *albumHeader;
    wxStaticText *artistHeader;
    wxStaticText *playlistHeader;
    TrackWindow *trackWindow;
    AlbumWindow *albumWindow;
    ArtistWindow *artistWindow;
    PlaylistWindow *playlistWindow;
    wxBitmapToggleButton *trackButton;
    wxBitmapToggleButton *albumButton;
    wxBitmapToggleButton *artistButton;
    wxBitmapToggleButton *playlistButton;

  public:
    SpotifyWindow(wxWindow *_parent, Downloader *_downloader);
    ~SpotifyWindow();

    inline TrackWindow *get_trackWindow() const { return trackWindow; };
    inline AlbumWindow *get_albumWindow() const { return albumWindow; };

    void search(const wxString &_searchText);
    void showSearchResults(const Downloader::SearchResult &result);
    void loadAdditionalSearchResults(const wxString &_type);
    void startDownload(wxCommandEvent &_event);
    void verifyTags(wxCommandEvent &_event);
}

;

#endif // SPOTIFY_WINDOW_H
