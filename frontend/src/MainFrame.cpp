#include "../include/MainFrame.h"
#include "../include/TrackEditWindow.h"
#include <taglib/mpegfile.h>

enum {
    IDM_TOOLBAR_DOWNLOAD = 200,
    IDM_TOOLBAR_SPOTIFY,
    IDM_TOOLBAR_YOUTUBE,
    IDM_TOOLBAR_SETTINGS
};

wxDEFINE_EVENT(EVT_SHOW_TRACK_DETAILS, wxCommandEvent);

// Constructor for main window
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "ID3 Auto Tagger 2", wxDefaultPosition,
              wxSize(800, 600)) {
    wxInitAllImageHandlers();

    auto toolBar = CreateToolBar(wxVERTICAL, wxID_ANY);
    toolBar->SetToolBitmapSize(wxSize(32, 32));
    toolBar->SetMinSize(wxSize(32 + 10, -1)); // Add a little padding if needed

    toolBar->AddTool(
        IDM_TOOLBAR_DOWNLOAD, wxEmptyString,
        wxBitmap(wxT("img/cloud-download-alt.png"), wxBITMAP_TYPE_PNG),
        wxNullBitmap, wxITEM_NORMAL, wxT("Download"),
        wxT("View Downloaded Tracks"));
    toolBar->AddTool(IDM_TOOLBAR_SPOTIFY, wxEmptyString,
                     wxBitmap(wxT("img/spotify.png"), wxBITMAP_TYPE_PNG),
                     wxNullBitmap, wxITEM_NORMAL, wxT("Spotify"),
                     wxT("Open Spotify Screen"));
    toolBar->AddTool(IDM_TOOLBAR_YOUTUBE, wxEmptyString,
                     wxBitmap(wxT("img/youtube.png"), wxBITMAP_TYPE_PNG),
                     wxNullBitmap, wxITEM_NORMAL, wxT("YouTube"),
                     wxT("Open YouTube Screen"));
    toolBar->AddTool(IDM_TOOLBAR_SETTINGS, wxEmptyString,
                     wxBitmap(wxT("img/customize.png"), wxBITMAP_TYPE_PNG),
                     wxNullBitmap, wxITEM_NORMAL, wxT("Settings"),
                     wxT("Open Settings Screen"));
    toolBar->Realize();

    // Create the main panel that will switch content
    mainPanel = new wxPanel(this);
    mainSizer = new wxBoxSizer(wxVERTICAL);
    mainPanel->SetSizer(mainSizer);

    // Create panels for different screens (Initially hidden)
    downloadPanel = new TrackWindow(mainPanel);
    spotifyPanel = new TrackWindow(mainPanel);
    youtubePanel = new TrackWindow(mainPanel);
    settingsPanel = new SettingsWindow(mainPanel);

    // mainPanel->SetBackgroundColour(*wxBLUE);
    // downloadPanel->SetBackgroundColour(*wxYELLOW);
    // spotifyPanel->SetBackgroundColour(*wxGREEN);
    // youtubePanel->SetBackgroundColour(*wxRED);

    wxTextCtrl *searchBar =
        new wxTextCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition,
                       wxDefaultSize, wxTE_PROCESS_ENTER);
    searchBar->Bind(wxEVT_TEXT_ENTER, [this, searchBar](wxCommandEvent &event) {
        this->search(searchBar->GetValue());
    });
    wxBitmapButton *searchButton = new wxBitmapButton(
        mainPanel, wxID_ANY,
        wxBitmap(wxT("img/magnifying-glass-play.png"), wxBITMAP_TYPE_PNG),
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    searchButton->Bind(wxEVT_BUTTON, [this, searchBar](wxCommandEvent &event) {
        this->search(searchBar->GetValue());
    });

    auto searchSizer = new wxBoxSizer(wxHORIZONTAL);
    searchSizer->Add(searchBar, 1, wxEXPAND, 5);
    searchSizer->Add(searchButton, 0, wxEXPAND, 5);
    mainSizer->Add(searchSizer, 0, wxEXPAND, 5);
    mainSizer->Add(downloadPanel, 1, wxEXPAND);
    mainSizer->Add(spotifyPanel, 1, wxEXPAND);
    mainSizer->Add(youtubePanel, 1, wxEXPAND);

    spotifyPanel->Hide();
    youtubePanel->Hide();
    downloadPanel->Hide();

    // Set default screen
    ShowPanel(downloadPanel);

    auto trackEditWindow = new TrackEditWindow(this);
    trackEditWindow->Hide();

    // Bind toolbar events
    Bind(wxEVT_TOOL, &MainFrame::OnDownloadClicked, this, IDM_TOOLBAR_DOWNLOAD);
    Bind(wxEVT_TOOL, &MainFrame::OnSpotifyClicked, this, IDM_TOOLBAR_SPOTIFY);
    Bind(wxEVT_TOOL, &MainFrame::OnYouTubeClicked, this, IDM_TOOLBAR_YOUTUBE);
    Bind(wxEVT_TOOL, &MainFrame::OnSettingsClicked, this, IDM_TOOLBAR_SETTINGS);

    this->Bind(EVT_TRACK_DOWNLOAD, &MainFrame::startDownload, this);

    this->Bind(EVT_SHOW_TRACK_DETAILS,
               [this, trackEditWindow](wxCommandEvent &event) {
                   TrackWindow *trackWindow =
                       dynamic_cast<TrackWindow *>(event.GetEventObject());
                   if (!trackWindow) {
                       wxLogMessage("Could not derive TrackWindow from %s",
                                    event.GetEventObject());
                       return;
                   }
                   std::vector<TagLib::MPEG::File *> tracks;
                   for (auto &&activeSong : trackWindow->get_activeSongs()) {
                       tracks.push_back(activeSong->get_localTrack());
                   }
                   if (tracks.empty()) {
                       trackEditWindow->Hide();
                       return;
                   }
                   trackEditWindow->Show();
                   trackEditWindow->GetParent()->Layout();
                   trackEditWindow->show(tracks);
               });
    this->Bind(EVT_VALUE_CHANGE, [this](wxCommandEvent &event) {
        std::string value = event.GetString().ToStdString();
        Spotify::Track::tag_type_t type =
            static_cast<Spotify::Track::tag_type_t>(event.GetInt());
        for (auto &&activeSong : downloadPanel->get_activeSongs()) {
            auto track = activeSong->get_localTrack();
            if (!track)
                return;
            switch (type) {
            case Spotify::Track::TITLE: {
                track->tag()->setTitle(value);
                break;
            }
            case Spotify::Track::ARTIST: {
                track->tag()->setArtist(value);
                break;
            }
            case Spotify::Track::ALBUM: {
                track->tag()->setAlbum(value);
                break;
            }
            case Spotify::Track::ALBUM_ARTIST: {
                Spotify::Track::setTagValue(*track, "TPE2", value);
                break;
            }
            case Spotify::Track::GENRE: {
                track->tag()->setGenre(value);
                break;
            }
            case Spotify::Track::LABEL: {
                Spotify::Track::setTagValue(*track, "TPUB", value);
                break;
            }
            case Spotify::Track::YEAR: {
                track->tag()->setYear(std::stoi(value));

                break;
            }
            case Spotify::Track::TRACK: {
                track->tag()->setTrack(std::stoi(value));
                break;
            }
            case Spotify::Track::DISC: {
                Spotify::Track::setTagValue(*track, "TPOS", value);
                break;
            }
            }
            track->save();
        }
        refreshDownloaded();
    });

    // Set main layout
    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(toolBar, 0, wxSHRINK, 0);
    sizer->Add(mainPanel, 1, wxEXPAND);
    sizer->Add(trackEditWindow, 0, wxSHRINK, 5);
    SetSizer(sizer);
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
    ShowPanel(spotifyPanel);
}

void MainFrame::OnYouTubeClicked(wxCommandEvent &event) {
    ShowPanel(youtubePanel);
}

void MainFrame::OnSettingsClicked(wxCommandEvent &event) {
    ShowPanel(settingsPanel);
}

void MainFrame::startDownload(wxCommandEvent &_event) {
    TrackLabel *chosenTrackLabel =
        spotifyPanel->get_trackLabels().at(_event.GetString().ToStdString());
    Spotify::Track chosenTrack = *chosenTrackLabel->get_spotifyTrack();
    downloader.downloadResource(
        {chosenTrack}, [chosenTrackLabel](int progress) {
            wxLogDebug("%i",
                       progress); // yt-dlp output should be inserted here
            chosenTrackLabel->get_ProgressBar()->SetProgress(progress);
        });
}

void MainFrame::refreshDownloaded() {
    std::filesystem::path musicPath = std::filesystem::current_path() / "music";

    // Check if the directory exists
    if (!std::filesystem::exists(musicPath) ||
        !std::filesystem::is_directory(musicPath)) {
        wxMessageBox("Music directory not found!", "Error", wxICON_ERROR);
        return;
    }
    downloadPanel->deleteChildren();

    // Iterate through the files in the directory
    for (const auto &file : std::filesystem::directory_iterator(musicPath)) {
        if (file.is_regular_file()) { // Only add files, not directories
            downloadPanel->appendChildren(
                new TrackLabel(downloadPanel, file.path().string()));
        }
    }
// downloadPanel->ge
    downloadPanel->FitInside();
    downloadPanel->Layout();
    ShowPanel(downloadPanel);
}

void MainFrame::search(const wxString &_searchText) {
    std::vector<Spotify::Track> tracks;
    std::string origin =
        downloader.fetchResource(_searchText.ToStdString(), tracks);
    TrackWindow *activePanel;
    if (origin == "Spotify") {
        activePanel = spotifyPanel;
    } else if (origin == "YouTube") {
        activePanel = youtubePanel;
    } else
        return;
    activePanel->deleteChildren();
    for (auto &&track : tracks) {
        activePanel->appendChildren(new TrackLabel(activePanel, track));
    }
    activePanel->FitInside();
    activePanel->Layout();
    ShowPanel(activePanel);
}
