#if !defined(SPOTIFY_WINDOW_H)
#define SPOTIFY_WINDOW_H

#include <vector>
#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "Components/MediaLabel.h"
#include "Interfaces/ISearchResult.hpp"
#include "Media/Album/AlbumDetailsPanel.h"
#include "Windows/MediaWindow.hpp"

class TrackPanel;
class MediaServiceRegistry;

class SpotifyWindow : public wxScrolledWindow {
  private:
    MediaServiceRegistry *registry;

    wxStaticText *trackHeader;
    wxStaticText *albumHeader;
    wxStaticText *artistHeader;
    wxStaticText *playlistHeader;
    TrackPanel *trackPanel;
    MediaWindow<MediaLabel> *albumWindow;
    MediaWindow<MediaLabel> *artistWindow;
    MediaWindow<MediaLabel> *playlistWindow;
    MediaWindow<MediaLabel> *videoWindow;

    AlbumDetailsPanel *albumDetailsPanel;

    wxBitmapToggleButton *trackButton;
    wxBitmapToggleButton *albumButton;
    wxBitmapToggleButton *artistButton;
    wxBitmapToggleButton *playlistButton;
    wxBitmapToggleButton *videoButton;

    wxBitmapToggleButton *discogsButton;
    wxBitmapToggleButton *spotifyButton;
    wxBitmapToggleButton *youtubeButton;

  public:
    SpotifyWindow(wxWindow *_parent, MediaServiceRegistry *_registry);
    ~SpotifyWindow();

    void search(const wxString &_searchText);
    void showSearchResults(const std::vector<ISearchResult> &_results);
    void loadAdditionalSearchResults(const wxString &_type);
    void startDownload(wxCommandEvent &_event);
    void verifyTags(wxCommandEvent &_event);
};

#endif // SPOTIFY_WINDOW_H