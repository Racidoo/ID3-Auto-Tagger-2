#if !defined(MAINFRAME_H)
#define MAINFRAME_H

// #include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <wx/wx.h>

class Downloader;
class SettingsWindow;
class SpotifyWindow;
class TrackWindow;
class LocalTrackWindow;

// Define a main window class
class MainFrame : public wxFrame {
  public:
    MainFrame();
    ~MainFrame() = default;

  private:
    std::unique_ptr<Downloader> downloader;
    wxBoxSizer *mainSizer;
    wxPanel *mainPanel;
    SettingsWindow *settingsPanel;
    SpotifyWindow *spotifyPanel;
    TrackWindow *youtubePanel;
    LocalTrackWindow *downloadPanel;

    void ShowPanel(wxScrolledWindow *panel);
    void OnDownloadClicked(wxCommandEvent &event);
    void OnSpotifyClicked(wxCommandEvent &event);
    void OnYouTubeClicked(wxCommandEvent &event);
    void OnSettingsClicked(wxCommandEvent &event);

    void refreshDownloaded();
};

#endif // MAINFRAME_H
