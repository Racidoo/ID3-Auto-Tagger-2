#include "Windows/SpotifyWindow.h"
#include "Core/IconProvider.h"
#include "Media/Track/TrackPanel.h"
#include "Services/DownloadService.h"
#include "Services/MediaServiceRegistry.h"
#include "Services/TagService.h"

enum {
    IDM_TOOLBAR_ALL = 200,
    IDM_TOOLBAR_TRACK,
    IDM_TOOLBAR_ALBUM,
};

SpotifyWindow::SpotifyWindow(wxWindow *_parent, MediaServiceRegistry *_registry)
    : wxScrolledWindow(_parent), registry(_registry) {
    this->SetScrollRate(15, 15);

    if (!registry || !registry->get_searchService()) {
        wxLogInfo(wxT("Spotify service is currently unavailable!"));
        return;
    }

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
    videoButton = new wxBitmapToggleButton(
        this, wxID_ANY, wxArtProvider::GetBitmap(wxART_VIDEO, wxART_TOOLBAR));
    videoButton->SetValue(true);

    discogsButton = new wxBitmapToggleButton(
        this, wxID_ANY,
        wxArtProvider::GetBitmap(wxART_BRAND_DISCOGS, wxART_TOOLBAR));
    discogsButton->SetValue(true);
    spotifyButton = new wxBitmapToggleButton(
        this, wxID_ANY,
        wxArtProvider::GetBitmap(wxART_BRAND_SPOTIFY, wxART_TOOLBAR));
    spotifyButton->SetValue(true);
    youtubeButton = new wxBitmapToggleButton(
        this, wxID_ANY,
        wxArtProvider::GetBitmap(wxART_BRAND_YOUTUBE, wxART_TOOLBAR));
    youtubeButton->SetValue(true);

    toolbarSizer->Add(trackButton, 0, wxALL, 2);
    toolbarSizer->Add(albumButton, 0, wxALL, 2);
    toolbarSizer->Add(artistButton, 0, wxALL, 2);
    toolbarSizer->Add(playlistButton, 0, wxALL, 2);
    toolbarSizer->Add(videoButton, 0, wxALL, 2);
    toolbarSizer->Add(discogsButton, 0, wxALL, 2);
    toolbarSizer->Add(spotifyButton, 0, wxALL, 2);
    toolbarSizer->Add(youtubeButton, 0, wxALL, 2);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    trackPanel = new TrackPanel(this);
    albumWindow = new MediaWindow<MediaLabel>(this);
    artistWindow = new MediaWindow<MediaLabel>(this);
    playlistWindow = new MediaWindow<MediaLabel>(this);
    videoWindow = new MediaWindow<MediaLabel>(this);

    mainSizer->Add(searchSizer, 0, wxEXPAND, 5);
    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(trackPanel, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(albumWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(artistWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(playlistWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(videoWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

    trackPanel->Hide();
    albumWindow->Hide();
    artistWindow->Hide();
    playlistWindow->Hide();
    videoWindow->Hide();

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
                            "Albumdetails: " + album->get_name().value(),
                            wxDefaultPosition, wxSize(800, 600),
                            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
            dialog.SetMinSize(wxSize(800, 600));
            auto sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(new AlbumDetailsPanel(&dialog, album), 1,
                       wxEXPAND | wxALL, 5);
            dialog.Bind(EVT_TRACK_DOWNLOAD, &SpotifyWindow::startDownload,
                        this);

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
    // if (!downloader) {
    //     std::cerr << "Downloader not fully initialized" << std::endl;
    //     return;
    // }
    SearchOptions options;

    if (trackButton->GetValue())
        options.categories.insert(ISearchResult::SearchCategory::Track);
    if (albumButton->GetValue())
        options.categories.insert(ISearchResult::SearchCategory::Album);
    if (artistButton->GetValue())
        options.categories.insert(ISearchResult::SearchCategory::Artist);
    if (playlistButton->GetValue())
        options.categories.insert(ISearchResult::SearchCategory::Playlist);
    if (videoButton->GetValue())
        options.categories.insert(ISearchResult::SearchCategory::Video);

    // if (discogsButton->GetValue())
    //     options.sources.insert(IMediaService::MediaSourceId::Discogs);
    if (spotifyButton->GetValue())
        options.sources.insert(IMediaService::MediaSourceId::Spotify);
    // if (youtubeButton->GetValue())
    //     options.sources.insert(IMediaService::MediaSourceId::YouTube);

    std::vector<ISearchResult> results;
    if (registry->get_searchService()->supports(_searchText.ToStdString())) {
        results = {
            registry->get_searchService()->resolve(_searchText.ToStdString())};
    } else {
        results = registry->get_searchService()->search(
            _searchText.ToStdString(), options);
    }

    showSearchResults(results);
}

void SpotifyWindow::showSearchResults(
    const std::vector<ISearchResult> &_results) {

    albumWindow->deleteChildren();
    trackPanel->MergeTracks({});
    artistWindow->deleteChildren();
    playlistWindow->deleteChildren();

    for (auto &&result : _results) {

        if (!result.tracks.empty()) {
            trackPanel->Show();
            trackPanel->MergeTracks(result.tracks);

        } else {
            // trackPanel->Hide();
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
            // albumWindow->Hide();
        }
        if (!result.artists.empty()) {
            artistWindow->Show();
            for (const auto &artist : result.artists)
                artistWindow->appendChildren(
                    new MediaLabel(artistWindow, artist, {wxT("Artist")}));
        } else {
            // artistWindow->Hide();
        }
        if (!result.playlists.empty()) {
            playlistWindow->Show();
            for (const auto &playlist : result.playlists)
                playlistWindow->appendChildren(new MediaLabel(
                    playlistWindow, playlist, {wxT("Playlist")}));
        } else {
            // playlistWindow->Hide();
        }
        if (!result.videos.empty()) {
            videoWindow->Show();
            for (const auto &video : result.videos)
                videoWindow->appendChildren(
                    new MediaLabel(videoWindow, video, {wxT("Videos")}));
        } else {
            // playlistWindow->Hide();
        }
        FitInside();
        Layout();
    }
}

void SpotifyWindow::startDownload(wxCommandEvent &_event) {
    auto rows = trackPanel->GetSelectedRows();

    for (auto row : rows) {
        auto track = trackPanel->GetTrack(row);

        std::filesystem::path file =
            registry->get_trackPath() / std::string(track->get_id() + ".mp3");

        std::string youtubeId =
            TagService::researchVideoId(track, registry->get_searchService());

        if (youtubeId.empty()) {
            continue;
        }

        DownloadService::downloadAudio(
            track, youtubeId, file, *registry->get_local(),
            *registry->get_trackVerificationIndex(), [this, row](int progress) {
                trackPanel->SetDownloadProgress(row, progress);
            });
    }
    registry->get_trackVerificationIndex()->writeIndex();
}
