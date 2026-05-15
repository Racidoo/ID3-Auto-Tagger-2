#include "Windows/SpotifyWindow.h"
#include "Core/IconProvider.h"
#include "Media/Track/TrackPanel.h"

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

    this->Bind(EVT_TRACK_DOWNLOAD, &SpotifyWindow::startDownload, this);
    // this->Bind(EVT_TRACK_VERIFY, &SpotifyWindow::verifyTags, this);
    this->Bind(EVT_MEDIA_LABEL_CLICKED, [this](wxCommandEvent &event) {
        auto *label = dynamic_cast<MediaLabel *>(event.GetEventObject());
        auto source = label->get_source();

        if (!source) {
            wxLogDebug(wxT("invalid reference to source"));
        }
        if (auto artist = std::dynamic_pointer_cast<IArtist>(source)) {
            wxLogDebug(wxT("artist"));
        } else if (auto album = std::dynamic_pointer_cast<IAlbum>(source)) {
            wxLogDebug(wxT("album"));
            wxDialog dialog(this, wxID_ANY,
                            "Albumdetails: " + album->get_title(),
                            wxDefaultPosition, wxSize(800, 600),
                            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
            dialog.SetMinSize(wxSize(800, 600));
            auto sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(new AlbumDetailsPanel(&dialog, album, downloader), 1,
                       wxEXPAND | wxALL, 5);
            dialog.SetSizer(sizer);
            dialog.Layout();
            dialog.SetSize(800, 600);
            dialog.Centre();
            dialog.ShowModal();

        } else if (auto playlist =
                       std::dynamic_pointer_cast<IPlaylist>(source)) {
            wxLogDebug(wxT("playlist"));
        }
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
    std::set<ISearchResult::SearchCategory> activeCategories;

    if (trackButton->GetValue())
        activeCategories.insert(ISearchResult::SearchCategory::Track);
    if (albumButton->GetValue())
        activeCategories.insert(ISearchResult::SearchCategory::Album);
    if (artistButton->GetValue())
        activeCategories.insert(ISearchResult::SearchCategory::Artist);
    if (playlistButton->GetValue())
        activeCategories.insert(ISearchResult::SearchCategory::Playlist);

    ISearchResult result = downloader->fetchResource(_searchText.ToStdString(),
                                                     activeCategories, "DE", 5);
    showSearchResults(result);
}

void SpotifyWindow::showSearchResults(ISearchResult &result) {

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
                {wxString(album->get_artist()), wxString(album->get_year())}));
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

void SpotifyWindow::loadAdditionalSearchResults(const wxString &_type) {
    std::set<ISearchResult::SearchCategory> activeCategories;

    if (_type == "track")
        activeCategories.insert(ISearchResult::SearchCategory::Track);
    if (_type == "album")
        activeCategories.insert(ISearchResult::SearchCategory::Album);
    if (_type == "artist")
        activeCategories.insert(ISearchResult::SearchCategory::Artist);
    if (_type == "playlist")
        activeCategories.insert(ISearchResult::SearchCategory::Playlist);
}

void SpotifyWindow::startDownload(wxCommandEvent &_event) {
    if (!downloader) {
        std::cerr << "Downloader not fully initialized" << std::endl;
        return;
    }
    auto rows = trackPanel->GetSelectedRows();

    for (auto row : rows) {
        auto track = trackPanel->GetTrack(row);

        downloader->downloadResource(track, [this, row](int progress) {
            trackPanel->SetDownloadProgress(row, progress);
        });
    }
}
