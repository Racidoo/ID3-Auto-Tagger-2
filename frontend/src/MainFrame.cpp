#include "../include/MainFrame.h"

enum {
    IDM_TOOLBAR_DOWNLOAD = 200,
    IDM_TOOLBAR_SPOTIFY,
    IDM_TOOLBAR_YOUTUBE,
};

// Constructor for main window
MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Hello wxWidgets!", wxDefaultPosition,
              wxSize(800, 600)) {
    wxInitAllImageHandlers();

    auto toolBar = CreateToolBar(wxTB_LEFT, wxID_ANY);

    toolBar->AddTool(IDM_TOOLBAR_DOWNLOAD, "",
                     wxBitmap(wxT("img/cloud-download-alt.png"), wxBITMAP_TYPE_PNG),
                     wxNullBitmap, wxITEM_NORMAL, wxT("Download"),
                     wxT("View Downloaded Tracks"));
    toolBar->AddTool(IDM_TOOLBAR_SPOTIFY, "",
                     wxBitmap(wxT("img/spotify.png"), wxBITMAP_TYPE_PNG),
                     wxNullBitmap, wxITEM_NORMAL, wxT("Spotify"),
                     wxT("Open Spotify Screen"));

    toolBar->AddTool(IDM_TOOLBAR_YOUTUBE, "",
                     wxBitmap(wxT("img/youtube.png"), wxBITMAP_TYPE_PNG),
                     wxNullBitmap, wxITEM_NORMAL, wxT("YouTube"),
                     wxT("Open YouTube Screen"));

    toolBar->Realize();

    // Create the main panel that will switch content
    mainPanel = new wxScrolledWindow(this);
    mainSizer = new wxBoxSizer(wxVERTICAL);
    mainPanel->SetSizer(mainSizer);

    // Create panels for different screens (Initially hidden)
    downloadPanel = new TrackWindow(mainPanel);
    spotifyPanel = new TrackWindow(mainPanel);
    youtubePanel = new TrackWindow(mainPanel);

    wxTextCtrl *searchBar = new wxTextCtrl(mainPanel, wxID_ANY);
    wxBitmapButton *searchButton = new wxBitmapButton(
        mainPanel, wxID_ANY,
        wxBitmap(wxT("img/magnifying-glass-play.png"), wxBITMAP_TYPE_PNG),
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    searchButton->Bind(wxEVT_BUTTON, [this, searchBar](wxCommandEvent &event) {
        std::vector<Spotify::Track> tracks;
        std::string origin = downloader.fetchResource(
            searchBar->GetValue().ToStdString(), tracks);
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

        activePanel->GetSizer()->Layout();
        activePanel->Layout();
        activePanel->Fit();
        ShowPanel(activePanel);
    });

    auto searchSizer = new wxBoxSizer(wxHORIZONTAL);
    searchSizer->Add(searchBar, 1, wxEXPAND, 5);
    searchSizer->Add(searchButton, 0, wxEXPAND, 5);
    mainSizer->Add(searchSizer, 0, wxEXPAND, 5);
    mainSizer->Add(downloadPanel, 1, wxEXPAND);
    mainSizer->Add(spotifyPanel, 1);
    mainSizer->Add(youtubePanel, 1);

    spotifyPanel->Hide();
    youtubePanel->Hide();
    downloadPanel->Hide();

    // Set default screen
    ShowPanel(downloadPanel);

    // Bind toolbar events
    Bind(wxEVT_TOOL, &MainFrame::OnDownloadClicked, this, IDM_TOOLBAR_DOWNLOAD);
    Bind(wxEVT_TOOL, &MainFrame::OnSpotifyClicked, this, IDM_TOOLBAR_SPOTIFY);
    Bind(wxEVT_TOOL, &MainFrame::OnYouTubeClicked, this, IDM_TOOLBAR_YOUTUBE);

    this->Bind(EVT_TRACK_DOWNLOAD, &MainFrame::onTrackAction, this);

    // Set main layout
    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(toolBar, 0, wxEXPAND);
    sizer->Add(mainPanel, 1, wxEXPAND);
    SetSizer(sizer);
}

// Button click event handler
void MainFrame::OnButtonClick(wxCommandEvent &event) {
    // wxMessageBox("Hello, World!", "Message", wxOK | wxICON_INFORMATION);

    // auto result = wxGetTextFromUser("Enter text", "another text");
    // te

    // try {
    //     Spotify::SpotifyAPI spotify;
    //     YouTube youTube;

    // const std::string url = (*topMatches[0])
    //                             .at("album")
    //                             .at("images")
    //                             .at(1)
    //                             .at("url")
    //                             .get<std::string>();
    // spotify.downloadImage(
    //     "https://i.scdn.co/image/ab67616d00001e02e1de2825be43ae39f4567db2",
    //     "cover.png");

    // wxFileDialog openFileDialog(this, "Choose a file", "", "",
    //                             "MP3 Files (*.mp3*)|*.mp3*",
    //                             wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    // if (openFileDialog.ShowModal() == wxID_OK) {
    //     auto filename = openFileDialog.GetPath();
    //     std::cout << filename << std::endl;
    //     wxMessageBox("Selected file: " + filename, "File Selected",
    //                  wxOK | wxICON_INFORMATION);

    //     std::cout << "Match: "
    //               << spotify.searchId(std::string(filename.mb_str()),
    //                                   "track")
    //               << std::endl;
    // }

    // std::string id = "0agQ9vIV7NP4dntGKLcCXO";

    // std::string bestMatch = youTube.findBestMatch(spotify.getTrack(id));

    // std::string command =
    //     "yt-dlp -f bestaudio --extract-audio --audio-format "
    //     "mp3 --audio-quality 0 -o '" +
    //     id + ".mp3' https://www.youtube.com/watch?v=" + bestMatch;
    // std::system(command.c_str());
    // std::cout << command << std::endl;

    // } catch (const std::exception &e) {
    //     std::cerr << e.what() << '\n';
    // }
}

void MainFrame::ShowPanel(wxScrolledWindow *panel) {
    downloadPanel->Hide();
    spotifyPanel->Hide();
    youtubePanel->Hide();
    panel->Show();
    mainSizer->Layout();
}

void MainFrame::OnDownloadClicked(wxCommandEvent &event) {
    refreshDownloaded();
    ShowPanel(downloadPanel);
}

void MainFrame::OnSpotifyClicked(wxCommandEvent &event) {
    ShowPanel(spotifyPanel);
}

void MainFrame::OnYouTubeClicked(wxCommandEvent &event) {
    ShowPanel(youtubePanel);
}

void MainFrame::refreshDownloaded() {
    downloadPanel->GetSizer()->Clear(true); // true deletes child widgets
    std::filesystem::path musicPath = std::filesystem::current_path() / "music";

    // Check if the directory exists
    if (!std::filesystem::exists(musicPath) ||
        !std::filesystem::is_directory(musicPath)) {
        wxMessageBox("Music directory not found!", "Error", wxICON_ERROR);
        return;
    }

    // Iterate through the files in the directory
    for (const auto &file : std::filesystem::directory_iterator(musicPath)) {
        if (file.is_regular_file()) { // Only add files, not directories
            auto trackLabel =
                new TrackLabel(downloadPanel, file.path().string());
            downloadPanel->GetSizer()->Add(trackLabel, 0, wxEXPAND);
        }
    }

    // Update the layout
    downloadPanel->GetSizer()->Layout();
    downloadPanel->Layout();
    downloadPanel->Fit();
}

void MainFrame::onTrackAction(wxCommandEvent &_event) {
    std::cout << "download " << _event.GetString() << std::endl;

    std::string id = _event.GetString().ToStdString();

    TrackLabel *chosenTrackLabel = spotifyPanel->get_trackLabels().at(id);
    Spotify::Track chosenTrack = *chosenTrackLabel->get_spotifyTrack();
    downloader.downloadResource({chosenTrack});
    chosenTrackLabel->SetBackgroundColour(*wxGREEN);
}
