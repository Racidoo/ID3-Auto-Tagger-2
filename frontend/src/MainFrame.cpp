#include "../include/MainFrame.h"
#include "../include/Downloader.h"
#include "../include/IconProvider.h"
#include "../include/LabeledTextCtrl.h"
#include "../include/LocalTrackWindow.h"
#include "../include/SettingsWindow.h"
#include "../include/SpotifyWindow.h"
#include "../include/TrackEditWindow.h"
#include "../include/TrackInterface.h"
#include "../include/TrackLabel.h"
#include "../include/TrackWindow.h"
#include "../include/YouTubeWindow.h"
#include <taglib/mpegfile.h>

enum {
    IDM_TOOLBAR_DOWNLOAD = 200,
    IDM_TOOLBAR_SPOTIFY,
    IDM_TOOLBAR_YOUTUBE,
    IDM_TOOLBAR_SETTINGS
};

// Constructor for main window
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "ID3 Auto Tagger 2", wxDefaultPosition,
              wxSize(800, 600)),
      downloader(std::make_unique<Downloader>()) {
    wxInitAllImageHandlers();

    auto toolBar = CreateToolBar(wxVERTICAL, wxID_ANY);
    toolBar->SetToolBitmapSize(wxSize(32, 32));
    toolBar->SetMinSize(wxSize(32 + 10, -1)); // Add a little padding if needed

    toolBar->AddTool(IDM_TOOLBAR_DOWNLOAD, wxEmptyString,
                     wxArtProvider::GetBitmap(wxART_DOWNLOADED, wxART_TOOLBAR),
                     wxNullBitmap, wxITEM_NORMAL, wxT("Download"),
                     wxT("View Downloaded Tracks"));
    toolBar->AddTool(
        IDM_TOOLBAR_SPOTIFY, wxEmptyString,
        wxArtProvider::GetBitmap(wxART_BRAND_SPOTIFY, wxART_TOOLBAR),
        wxNullBitmap, wxITEM_NORMAL, wxT("Spotify"),
        wxT("Open Spotify Screen"));
    toolBar->AddTool(
        IDM_TOOLBAR_YOUTUBE, wxEmptyString,
        wxArtProvider::GetBitmap(wxART_BRAND_YOUTUBE, wxART_TOOLBAR),
        wxNullBitmap, wxITEM_NORMAL, wxT("YouTube"),
        wxT("Open YouTube Screen"));
    toolBar->AddTool(IDM_TOOLBAR_SETTINGS, wxEmptyString,
                     wxArtProvider::GetBitmap(wxART_CUSTOMIZE, wxART_TOOLBAR),
                     wxNullBitmap, wxITEM_NORMAL, wxT("Settings"),
                     wxT("Open Settings Screen"));
    toolBar->Realize();

    // Create the main panel that will switch content
    mainPanel = new wxPanel(this);
    mainSizer = new wxBoxSizer(wxVERTICAL);
    mainPanel->SetSizer(mainSizer);

    // Create panels for different screens (Initially hidden)
    downloadPanel = new LocalTrackWindow(mainPanel, downloader.get());
    spotifyPanel = new SpotifyWindow(mainPanel, downloader.get());
    youtubePanel = new YouTubeWindow(mainPanel, downloader.get());
    settingsPanel = new SettingsWindow(mainPanel, downloader.get());

    mainSizer->Add(downloadPanel, 1, wxEXPAND);
    mainSizer->Add(spotifyPanel, 1, wxEXPAND);
    mainSizer->Add(youtubePanel, 1, wxEXPAND);
    mainSizer->Add(settingsPanel, 1, wxEXPAND);

    auto trackEditWindow = new TrackEditWindow(this);
    trackEditWindow->Hide();

    // Bind toolbar events
    Bind(wxEVT_TOOL, &MainFrame::OnDownloadClicked, this, IDM_TOOLBAR_DOWNLOAD);
    Bind(wxEVT_TOOL, &MainFrame::OnSpotifyClicked, this, IDM_TOOLBAR_SPOTIFY);
    Bind(wxEVT_TOOL, &MainFrame::OnYouTubeClicked, this, IDM_TOOLBAR_YOUTUBE);
    Bind(wxEVT_TOOL, &MainFrame::OnSettingsClicked, this, IDM_TOOLBAR_SETTINGS);

    // Set main layout
    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(toolBar, 0, wxSHRINK, 0);
    sizer->Add(mainPanel, 1, wxEXPAND);
    sizer->Add(trackEditWindow, 0, wxSHRINK, 5);
    SetSizer(sizer);

    // Set default screen
    if (!downloader->is_initialized()) {
        ShowPanel(settingsPanel);
    } else
        ShowPanel(downloadPanel);
}

void MainFrame::ShowPanel(wxScrolledWindow *panel) {
    downloadPanel->Hide();
    spotifyPanel->Hide();
    youtubePanel->Hide();
    settingsPanel->Hide();
    panel->Show();

    mainPanel->Layout();    // Trigger layout update
    mainPanel->FitInside(); // If it's a scrolled window, optional
}

void MainFrame::OnDownloadClicked(wxCommandEvent &event) {
    refreshDownloaded();
}

void MainFrame::OnSpotifyClicked(wxCommandEvent &event) {
    if (!downloader->is_initialized()) {
        wxLogError(wxT(
            "Provide credentials in the Settings, before using API-Services!"));
        return;
    }
    ShowPanel(spotifyPanel);
}

void MainFrame::OnYouTubeClicked(wxCommandEvent &event) {
    if (!downloader->is_initialized()) {
        wxLogError(wxT(
            "Provide credentials in the Settings, before using API-Services!"));
        return;
    }
    ShowPanel(youtubePanel);
}

void MainFrame::OnSettingsClicked(wxCommandEvent &event) {
    ShowPanel(settingsPanel);
}

void MainFrame::refreshDownloaded() {
    downloadPanel->refresh();
    downloadPanel->FitInside();
    downloadPanel->Layout();
    ShowPanel(downloadPanel);
}