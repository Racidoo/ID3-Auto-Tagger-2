#if !defined(SPOTIFY_WINDOW_H)
#define SPOTIFY_WINDOW_H

#include <vector>
#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "Downloader.h"
#include "MediaWindow.hpp"
#include "Interfaces/ISearchResult.hpp"

class AlbumLabel;
class ArtistLabel;
class PlaylistLabel;
class TrackPanel;

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
    TrackPanel *trackPanel;
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

    // inline TrackWindow *get_trackWindow() const { return trackWindow; };
    inline AlbumWindow *get_albumWindow() const { return albumWindow; };

    void search(const wxString &_searchText);
    void showSearchResults(ISearchResult &result);
    void loadAdditionalSearchResults(const wxString &_type);
    void startDownload(wxCommandEvent &_event);
    void verifyTags(wxCommandEvent &_event);
};

#endif // SPOTIFY_WINDOW_H