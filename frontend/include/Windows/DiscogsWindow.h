#pragma once

#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "Components/MediaLabel.h"
#include "Interfaces/ISearchResult.hpp"
#include "Windows/MediaWindow.hpp"

class Downloader;
class TrackPanel;

class DiscogsWindow : public wxScrolledWindow {
  private:
    Downloader *downloader;

    wxStaticText *trackHeader;
    wxStaticText *albumHeader;
    wxStaticText *artistHeader;
    wxStaticText *playlistHeader;
    TrackPanel *trackPanel;
    MediaWindow<MediaLabel> *albumWindow;
    MediaWindow<MediaLabel> *artistWindow;
    MediaWindow<MediaLabel> *playlistWindow;
    wxBitmapToggleButton *trackButton;
    wxBitmapToggleButton *albumButton;
    wxBitmapToggleButton *artistButton;
    wxBitmapToggleButton *playlistButton;

  public:
    DiscogsWindow(wxWindow *_parent, Downloader *_downloader);
    ~DiscogsWindow() = default;

    void search(const wxString &_searchText);
    void showSearchResults(ISearchResult &result);
    // void loadAdditionalSearchResults(const wxString &_type);
    // void startDownload(wxCommandEvent &_event);
    // void verifyTags(wxCommandEvent &_event);
};