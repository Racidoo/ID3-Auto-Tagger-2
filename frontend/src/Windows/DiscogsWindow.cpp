#include "Windows/DiscogsWindow.h"
#include "Core/IconProvider.h"
#include "Media/Track/TrackPanel.h"

DiscogsWindow::DiscogsWindow(wxWindow *_parent) : wxScrolledWindow(_parent) {
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

    trackPanel = new TrackPanel(this);
    albumWindow = new MediaWindow<MediaLabel>(this);
    artistWindow = new MediaWindow<MediaLabel>(this);
    playlistWindow = new MediaWindow<MediaLabel>(this);

    mainSizer->Add(searchSizer, 0, wxEXPAND, 5);
    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(trackPanel, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(albumWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(artistWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(playlistWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

    trackPanel->Hide();
    albumWindow->Hide();
    artistWindow->Hide();
    playlistWindow->Hide();

    SetSizerAndFit(mainSizer);

    // this->Bind(EVT_TRACK_DOWNLOAD, &SpotifyWindow::startDownload, this);
    // this->Bind(EVT_TRACK_VERIFY, &SpotifyWindow::verifyTags, this);
    // this->Bind(EVT_MEDIA_LABEL_CLICKED, [this](wxCommandEvent &event) {
    //     wxLogInfo(wxT("Temporarily disabled!"));
    //     // auto *label = dynamic_cast<MediaLabel *>(event.GetEventObject());
    //     // if (!label)
    //     //     return;
    //     // const QueryObject *obj = label->getObject();
    //     // if (!obj)
    //     //     return;

    //     // std::string id = obj->get_id();
    //     // std::string type = obj->get_type();

    //     // std::cout << "Clicked object ID: " << id << ", Type: " << type
    //     //           << std::endl;
    //     // // track search for artist not supported
    //     // if (type == "artist") {
    //     //     return;
    //     // }

    //     // std::string url("https://open.spotify.com/intl-de/" + type + "/" +
    //     // id); auto searchResults = downloader->fetchResource(url);
    //     // showSearchResults(searchResults);
    // });
    // this->Bind(EVT_MEDIA_WINDOW_EXPAND_CLICKED, [this](wxCommandEvent event)
    // {
    //     wxLogDebug("MediaWindow expand clicked. type: " + event.GetString());
    // });
}

void DiscogsWindow::search(const wxString &_searchText) {
    // if (!downloader || !downloader->initializeDiscogs()) {
    //     std::cerr << "Discogs not fully initialized" << std::endl;
    //     return;
    // }
    // Discogs::DiscogsAPI::SearchParams params{};
    // params.query = _searchText.ToStdString();

    // if (trackButton->GetValue())
    //     params.categories.insert(ISearchResult::SearchCategory::Track);
    // if (albumButton->GetValue())
    //     params.categories.insert(ISearchResult::SearchCategory::Album);
    // if (artistButton->GetValue())
    //     params.categories.insert(ISearchResult::SearchCategory::Artist);
    // if (playlistButton->GetValue())
    //     params.categories.insert(ISearchResult::SearchCategory::Playlist);

    // auto results = downloader->get_discogs()->search(params);

    // showSearchResults(results);
}

void DiscogsWindow::showSearchResults(ISearchResult &result) {

    albumWindow->deleteChildren();
    trackPanel->MergeTracks({});
    artistWindow->deleteChildren();
    playlistWindow->deleteChildren();

    if (!result.tracks.empty()) {
        trackPanel->Show();
        trackPanel->MergeTracks(result.tracks);

    } else {
        trackPanel->Hide();
    }

    if (!result.albums.empty()) {
        albumWindow->Show();
        for (auto album : result.albums)
            albumWindow->appendChildren(new MediaLabel(
                albumWindow, album,
                {wxString(album->get_artist().value()),
                 wxString(album->get_year().has_value()
                              ? std::to_string(album->get_year().value())
                              : "")}));
    } else {
        albumWindow->Hide();
    }
    if (!result.artists.empty()) {
        artistWindow->Show();
        for (const auto &artist : result.artists)
            artistWindow->appendChildren(
                new MediaLabel(artistWindow, artist, {wxT("Artist")}));
    } else {
        artistWindow->Hide();
    }
    if (!result.playlists.empty()) {
        playlistWindow->Show();
        for (const auto &playlist : result.playlists)
            playlistWindow->appendChildren(
                new MediaLabel(playlistWindow, playlist, {wxT("Playlist")}));
    } else {
        playlistWindow->Hide();
    }

    FitInside();
    Layout();
}