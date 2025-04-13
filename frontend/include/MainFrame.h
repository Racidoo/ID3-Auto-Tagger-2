#if !defined(MAINFRAME_H)
#define MAINFRAME_H

#include <filesystem>
#include <iostream>
#include <string>
#include <wx/wx.h>

#include "SettingsWindow.h"
#include "Spotify/SpotifyAPI.h"
#include "SpotifyWindow.h"
#include "TrackLabel.h"
#include "TrackWindow.h"

// Define a main window class
class MainFrame : public wxFrame {
  public:
    MainFrame();
    ~MainFrame() = default;

  private:
    wxBoxSizer *mainSizer;
    wxPanel *mainPanel;
    SettingsWindow *settingsPanel;
    SpotifyWindow *spotifyPanel;
    TrackWindow *youtubePanel;
    TrackWindow *downloadPanel;

    void ShowPanel(wxScrolledWindow *panel);
    void OnDownloadClicked(wxCommandEvent &event);
    void OnSpotifyClicked(wxCommandEvent &event);
    void OnYouTubeClicked(wxCommandEvent &event);
    void OnSettingsClicked(wxCommandEvent &event);

    void refreshDownloaded();
};

#endif // MAINFRAME_H
