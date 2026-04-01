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
    // toolBar->AddTool(IDM_TOOLBAR_YOUTUBE, wxEmptyString,
    //  wxArtProvider::GetBitmap(wxART_BRAND_YOUTUBE,wxART_TOOLBAR ),
    //                  wxNullBitmap, wxITEM_NORMAL, wxT("YouTube"),
    //                  wxT("Open YouTube Screen"));
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
    youtubePanel = new TrackWindow(mainPanel, downloader.get());
    settingsPanel = new SettingsWindow(mainPanel, downloader.get());

    mainSizer->Add(downloadPanel, 1, wxEXPAND);
    mainSizer->Add(spotifyPanel, 1, wxEXPAND);
    mainSizer->Add(youtubePanel, 1, wxEXPAND);
    mainSizer->Add(settingsPanel, 1, wxEXPAND);

    spotifyPanel->Hide();
    youtubePanel->Hide();
    downloadPanel->Hide();
    settingsPanel->Hide();

    auto trackEditWindow = new TrackEditWindow(this);
    trackEditWindow->Hide();

    // Bind toolbar events
    Bind(wxEVT_TOOL, &MainFrame::OnDownloadClicked, this, IDM_TOOLBAR_DOWNLOAD);
    Bind(wxEVT_TOOL, &MainFrame::OnSpotifyClicked, this, IDM_TOOLBAR_SPOTIFY);
    Bind(wxEVT_TOOL, &MainFrame::OnYouTubeClicked, this, IDM_TOOLBAR_YOUTUBE);
    Bind(wxEVT_TOOL, &MainFrame::OnSettingsClicked, this, IDM_TOOLBAR_SETTINGS);

    // this->Bind(
    //     EVT_SHOW_TRACK_DETAILS, [this, trackEditWindow](wxCommandEvent &event) {
    //         TrackWindow *trackWindow =
    //             dynamic_cast<TrackWindow *>(event.GetEventObject());
    //         if (!trackWindow) {
    //             wxLogMessage("Could not derive TrackWindow from %s",
    //                          event.GetEventObject());
    //             return;
    //         }
    //         std::vector<std::shared_ptr<TrackInterface::TrackViewData>> tracks;
    //         for (auto &&activeSong : trackWindow->get_activeSongs()) {
    //             tracks.push_back(activeSong->get_data());
    //         }
    //         if (tracks.empty()) {
    //             trackEditWindow->Hide();
    //             trackEditWindow->GetParent()->Layout();
    //             return;
    //         }
    //         trackEditWindow->Show();
    //         trackEditWindow->GetParent()->Layout();
    //         trackEditWindow->show(tracks);
    //     });
    // this->Bind(EVT_VALUE_CHANGE,
    //            [this, trackEditWindow](wxCommandEvent &event) {
    //                std::string value = event.GetString().ToStdString();
    //                LocalTrack::tag_type_t type =
    //                    static_cast<LocalTrack::tag_type_t>(event.GetInt());
    //                std::cout << "Caught event: " << value << " type: " << type
    //                          << std::endl;
    //                for (auto activeSong :
    //                     downloadPanel->get_trackWindow()->get_activeSongs()) {
    //                    auto track = activeSong->get_data()->local;
    //                    if (!track)
    //                        return;
    //                    switch (type) {
    //                    case LocalTrack::tag_type_t::TITLE: {
    //                        track->set_title(value);
    //                        break;
    //                    }
    //                    case LocalTrack::tag_type_t::ARTIST: {
    //                        track->set_artist(value);
    //                        break;
    //                    }
    //                    case LocalTrack::tag_type_t::ALBUM: {
    //                        track->set_album(value);
    //                        break;
    //                    }
    //                    case LocalTrack::tag_type_t::ALBUM_ARTIST: {
    //                        track->set_albumArtist(value);
    //                        break;
    //                    }
    //                    case LocalTrack::tag_type_t::COPYRIGHT: {
    //                        track->set_copyright(value);
    //                        break;
    //                    }
    //                    case LocalTrack::tag_type_t::GENRE: {
    //                        track->set_genre(value);
    //                        break;
    //                    }
    //                    case LocalTrack::tag_type_t::LABEL: {
    //                        track->set_label(value);
    //                        break;
    //                    }
    //                    case LocalTrack::tag_type_t::YEAR: {
    //                        track->set_year(value);
    //                        break;
    //                    }
    //                    case LocalTrack::tag_type_t::TRACK: {
    //                        track->set_track(value);
    //                        break;
    //                    }
    //                    case LocalTrack::tag_type_t::DISC: {
    //                        track->set_disc(value);
    //                        break;
    //                    }
    //                    }
    //                    activeSong->Update(TrackInterface::fromLocal(*track));
    //                }
    //            });

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