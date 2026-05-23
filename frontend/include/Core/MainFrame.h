#if !defined(MAINFRAME_H)
#define MAINFRAME_H

// #include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <wx/wx.h>

#include "Services/MediaServiceRegistry.h"

class SettingsWindow;
class SpotifyWindow;
class DiscogsWindow;
class LocalTrackWindow;
class YouTubeWindow;

// Define a main window class
class MainFrame : public wxFrame {
  public:
    MainFrame();
    ~MainFrame() = default;

  private:
    std::unique_ptr<MediaServiceRegistry> registry;
    wxBoxSizer *mainSizer;
    wxPanel *mainPanel;
    SettingsWindow *settingsPanel;
    SpotifyWindow *spotifyPanel;
    DiscogsWindow *discogsPanel;
    LocalTrackWindow *downloadPanel;
    YouTubeWindow *youtubePanel;

    void ShowPanel(wxScrolledWindow *panel);
    void OnDownloadClicked(wxCommandEvent &event);
    void OnSpotifyClicked(wxCommandEvent &event);
    void OnDiscogsClicked(wxCommandEvent &event);
    void OnYouTubeClicked(wxCommandEvent &event);
    void OnSettingsClicked(wxCommandEvent &event);

    void refreshDownloaded();
};

#endif // MAINFRAME_H
