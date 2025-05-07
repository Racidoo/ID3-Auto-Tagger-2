#include "../include/SpotifyWindow.h"
#include "../include/AlbumLabel.h"
#include "../include/ArtistLabel.h"
#include "../include/CircleProgressBar.h"
#include "../include/IconProvider.h"
#include "../include/PlaylistLabel.h"
#include "../include/TrackLabel.h"
#include "../include/TrackWindow.h"

enum {
    IDM_TOOLBAR_ALL = 200,
    IDM_TOOLBAR_TRACK,
    IDM_TOOLBAR_ALBUM,
};

SpotifyWindow::SpotifyWindow(wxWindow *_parent, Downloader *_downloader)
    : wxScrolledWindow(_parent), downloader(_downloader) {
    this->SetScrollRate(15, 15);

    auto toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

    wxTextCtrl *searchBar =
        new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                       wxDefaultSize, wxTE_PROCESS_ENTER);
    searchBar->Bind(wxEVT_TEXT_ENTER, [this, searchBar](wxCommandEvent &event) {
        this->search(searchBar->GetValue());
    });
    wxBitmapButton *searchButton = new wxBitmapButton(
        this, wxID_ANY, wxArtProvider::GetBitmap(wxART_SEARCH),
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    searchButton->Bind(wxEVT_BUTTON, [this, searchBar](wxCommandEvent &event) {
        this->search(searchBar->GetValue());
    });
    searchBar->SetFocus();

    auto searchSizer = new wxBoxSizer(wxHORIZONTAL);
    searchSizer->Add(searchBar, 1, wxEXPAND, 5);
    searchSizer->Add(searchButton, 0, wxEXPAND, 5);

    trackButton = new wxBitmapToggleButton(
        this, wxID_ANY, wxArtProvider::GetBitmap(wxART_TRACK, wxART_TOOLBAR));
    trackButton->SetValue(true);
    albumButton = new wxBitmapToggleButton(
        this, wxID_ANY, wxArtProvider::GetBitmap(wxART_ALBUM, wxART_TOOLBAR));
    albumButton->SetValue(true);
    artistButton = new wxBitmapToggleButton(
        this, wxID_ANY, wxArtProvider::GetBitmap(wxART_ARTIST, wxART_TOOLBAR));
    artistButton->SetValue(true);
    playlistButton = new wxBitmapToggleButton(
        this, wxID_ANY,
        wxArtProvider::GetBitmap(wxART_PLAYLIST, wxART_TOOLBAR));
    playlistButton->SetValue(true);

    toolbarSizer->Add(trackButton, 0, wxALL, 2);
    toolbarSizer->Add(albumButton, 0, wxALL, 2);
    toolbarSizer->Add(artistButton, 0, wxALL, 2);
    toolbarSizer->Add(playlistButton, 0, wxALL, 2);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    trackWindow = new TrackWindow(this, downloader);
    albumWindow = new AlbumWindow(this);
    artistWindow = new ArtistWindow(this);
    playlistWindow = new PlaylistWindow(this);

    mainSizer->Add(searchSizer, 0, wxEXPAND, 5);
    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(trackWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(albumWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(artistWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(playlistWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

    trackWindow->Hide();
    albumWindow->Hide();
    artistWindow->Hide();
    playlistWindow->Hide();

    SetSizerAndFit(mainSizer);

    this->Bind(EVT_TRACK_DOWNLOAD, &SpotifyWindow::startDownload, this);
    // this->Bind(EVT_TRACK_VERIFY, &SpotifyWindow::verifyTags, this);
    this->Bind(EVT_MEDIA_LABEL_CLICKED, [this](wxCommandEvent &event) {
        auto *label = dynamic_cast<MediaLabel *>(event.GetEventObject());
        if (!label)
            return;
        const Spotify::SpotifyObject *obj = label->getSpotifyObject();
        if (!obj)
            return;

        std::string id = obj->get_id();
        std::string type = obj->get_type();

        std::cout << "Clicked object ID: " << id << ", Type: " << type
                  << std::endl;
        // track search for artist not supported
        if (type == "artist") {
            return;
        }

        std::string url("https://open.spotify.com/intl-de/" + type + "/" + id);
        showSearchResults(downloader->fetchResource(url));
    });
    this->Bind(EVT_MEDIA_WINDOW_EXPAND_CLICKED, [this](wxCommandEvent event) {
        wxLogDebug("MediaWindow expand clicked. type: " + event.GetString());
    });
}

SpotifyWindow::~SpotifyWindow() {}

void SpotifyWindow::search(const wxString &_searchText) {
    if (!downloader) {
        std::cerr << "Downloader not fully initialized" << std::endl;
        return;
    }
    std::set<Downloader::SearchCategory> activeCategories;

    if (trackButton->GetValue())
        activeCategories.insert(Downloader::SearchCategory::Track);
    if (albumButton->GetValue())
        activeCategories.insert(Downloader::SearchCategory::Album);
    if (artistButton->GetValue())
        activeCategories.insert(Downloader::SearchCategory::Artist);
    if (playlistButton->GetValue())
        activeCategories.insert(Downloader::SearchCategory::Playlist);

    Downloader::SearchResult result =
        downloader->fetchResource(_searchText.ToStdString(), activeCategories);
    showSearchResults(result);
}

void SpotifyWindow::showSearchResults(const Downloader::SearchResult &result) {

    albumWindow->deleteChildren();
    trackWindow->deleteChildren();
    artistWindow->deleteChildren();
    playlistWindow->deleteChildren();

    if (!result.tracks.empty()) {
        trackWindow->Show();
        for (const auto &track : result.tracks)
            trackWindow->appendChildren(new TrackLabel(trackWindow, track));
    } else {
        trackWindow->Hide();
    }

    if (!result.albums.empty()) {
        albumWindow->Show();
        for (const auto &album : result.albums)
            albumWindow->appendChildren(new AlbumLabel(albumWindow, album));
    } else {
        albumWindow->Hide();
    }
    if (!result.artists.empty()) {
        artistWindow->Show();
        for (const auto &artist : result.artists)
            artistWindow->appendChildren(new ArtistLabel(artistWindow, artist));
    } else {
        artistWindow->Hide();
    }
    if (!result.playlists.empty()) {
        playlistWindow->Show();
        for (const auto &playlist : result.playlists)
            playlistWindow->appendChildren(
                new PlaylistLabel(playlistWindow, playlist));
    } else {
        playlistWindow->Hide();
    }

    FitInside();
    Layout();
}

void SpotifyWindow::loadAdditionalSearchResults(const wxString &_type) {
    std::set<Downloader::SearchCategory> activeCategories;

    if (_type == "track")
        activeCategories.insert(Downloader::SearchCategory::Track);
    if (_type == "album")
        activeCategories.insert(Downloader::SearchCategory::Album);
    if (_type == "artist")
        activeCategories.insert(Downloader::SearchCategory::Artist);
    if (_type == "playlist")
        activeCategories.insert(Downloader::SearchCategory::Playlist);
}

void SpotifyWindow::startDownload(wxCommandEvent &_event) {
    if (!downloader) {
        std::cerr << "Downloader not fully initialized" << std::endl;
        return;
    }
    TrackLabel *chosenTrackLabel =
        trackWindow->get_trackLabels().at(_event.GetString().ToStdString());
    Spotify::Track chosenTrack = *chosenTrackLabel->get_spotifyTrack();
    downloader->downloadResource(
        {chosenTrack}, [chosenTrackLabel](int progress) {
            chosenTrackLabel->get_ProgressBar()->SetProgress(progress);
        });
}

void SpotifyWindow::verifyTags(wxCommandEvent &_event) {
    wxLogDebug(wxT("verifyTags()"));
    wxLogDebug(wxT("Verify Tags of " + _event.GetString()));
    downloader->get_spotify()->verifyTags(_event.GetString().ToStdString());
}