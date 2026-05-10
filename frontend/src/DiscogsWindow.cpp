#include "DiscogsWindow.h"
#include "AlbumLabel.h"
#include "ArtistLabel.h"
#include "Downloader.h"
#include "IconProvider.h"
#include "PlaylistLabel.h"
#include "TrackPanel.h"

DiscogsWindow::DiscogsWindow(wxWindow *_parent, Downloader *_downloader)
    : wxScrolledWindow(_parent), downloader(_downloader) {
    this->SetScrollRate(15, 15);

    // auto toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

    // wxTextCtrl *searchBar =
    //     new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
    //                    wxDefaultSize, wxTE_PROCESS_ENTER);
    // searchBar->Bind(wxEVT_TEXT_ENTER, [this, searchBar](wxCommandEvent
    // &event) {
    //     this->search(searchBar->GetValue());
    // });
    // wxBitmapButton *searchButton = new wxBitmapButton(
    //     this, wxID_ANY, wxArtProvider::GetBitmap(wxART_SEARCH),
    //     wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    // searchButton->Bind(wxEVT_BUTTON, [this, searchBar](wxCommandEvent &event)
    // {
    //     this->search(searchBar->GetValue());
    // });
    // searchBar->SetFocus();

    // auto searchSizer = new wxBoxSizer(wxHORIZONTAL);
    // searchSizer->Add(searchBar, 1, wxEXPAND, 5);
    // searchSizer->Add(searchButton, 0, wxEXPAND, 5);

    // auto mainSizer = new wxBoxSizer(wxVERTICAL);
    // trackPanel = new TrackPanel(this, downloader);

    // mainSizer->Add(searchSizer, 0, wxEXPAND, 5);
    // mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    // mainSizer->Add(trackPanel, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

    // SetSizerAndFit(mainSizer);

    // this->Bind(EVT_MEDIA_LABEL_CLICKED, &DiscogsWindow::startDownload, this);

    // this->Bind(EVT_MEDIA_WINDOW_EXPAND_CLICKED, [this](wxCommandEvent event)
    // {
    //     wxLogDebug("MediaWindow expand clicked. type: " + event.GetString());
    // });

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

    // trackWindow = new TrackWindow(this, downloader);
    trackPanel = new TrackPanel(this, downloader);
    albumWindow = new AlbumWindow(this);
    artistWindow = new ArtistWindow(this);
    playlistWindow = new PlaylistWindow(this);

    mainSizer->Add(searchSizer, 0, wxEXPAND, 5);
    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(trackPanel, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    // mainSizer->Add(trackWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(albumWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(artistWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(playlistWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

    // trackWindow->Hide();
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
    if (!downloader || !downloader->initializeDiscogs()) {
        std::cerr << "Discogs not fully initialized" << std::endl;
        return;
    }
    Discogs::DiscogsAPI::SearchParams params{};
    params.query = _searchText.ToStdString();

    auto results = downloader->get_discogs()->search(params);
    // std::vector<std::shared_ptr<ITrack>> vec;
    // for (auto &&result : results) {
    //     vec.push_back(ITrack::fromDiscogs(
    //         std::make_shared<Discogs::ReleaseTrack>(result)));
    // }

    showSearchResults(results);
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