#pragma once

#include <wx/wx.h>

class Downloader;
class TrackPanel;

class DiscogsWindow : public wxScrolledWindow {
  private:
    Downloader *downloader;
    TrackPanel * trackPanel;

  public:
    DiscogsWindow(wxWindow *_parent, Downloader *_downloader);
    ~DiscogsWindow() = default;

    void search(const wxString &_searchText);
    // void showSearchResults(Downloader::SearchResult result);
    // void loadAdditionalSearchResults(const wxString &_type);
    // void startDownload(wxCommandEvent &_event);
    // void verifyTags(wxCommandEvent &_event);
};