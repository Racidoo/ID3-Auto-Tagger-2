#if !defined(MAINFRAME_H)
#define MAINFRAME_H

#include <filesystem>
#include <iostream>
#include <string>
#include <wx/wx.h>

class Downloader;
class SettingsWindow;
class SpotifyWindow;
class TrackWindow;

// Define a main window class
class MainFrame : public wxFrame {
  public:
    MainFrame();
    ~MainFrame() = default;

  private:
    Downloader *downloader;
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
