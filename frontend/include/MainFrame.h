#if !defined(MAINFRAME_H)
#define MAINFRAME_H

#include <iostream>
#include <string>
#include <wx/wx.h>

#include <filesystem>

#include "Downloader.h"
#include "SettingsWindow.h"
#include "Spotify/SpotifyAPI.h"
#include "TrackLabel.h"
#include "TrackWindow.h"
#include "YouTube.h"

// Define a main window class
class MainFrame : public wxFrame {
  public:
    MainFrame();
    ~MainFrame() = default;

  private:
    Downloader downloader;

    wxBoxSizer *mainSizer;
    // wxBoxSizer *downloadSizer;
    // wxBoxSizer *spotifySizer;
    // wxBoxSizer *youtubeSizer;
    wxPanel *mainPanel;
    SettingsWindow *settingsPanel;
    TrackWindow *spotifyPanel;
    TrackWindow *youtubePanel;
    TrackWindow *downloadPanel;

    void OnButtonClick(wxCommandEvent &event);

    void ShowPanel(wxScrolledWindow *panel);
    void OnDownloadClicked(wxCommandEvent &event);
    void OnSpotifyClicked(wxCommandEvent &event);
    void OnYouTubeClicked(wxCommandEvent &event);
    void OnSettingsClicked(wxCommandEvent &event);

    void refreshDownloaded();
    void search(const wxString &_searchText);

    void startDownload(wxCommandEvent &_event);
};

#endif // MAINFRAME_H
