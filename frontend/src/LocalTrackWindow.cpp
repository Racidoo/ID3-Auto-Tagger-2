#include "../include/LocalTrackWindow.h"
#include "../include/Downloader.h"
#include "../include/TrackEditWindow.h"
#include "../include/TrackLabel.h"
#include "../include/TrackWindow.h"

LocalTrackWindow::LocalTrackWindow(wxWindow *_parent, Downloader *_downloader)
    : wxScrolledWindow(_parent, wxID_ANY) {

    auto toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

    unverifiedCheckBox =
        new wxCheckBox(this, wxID_ANY, "Show unverified Tracks");
    unverifiedCheckBox->SetValue(true);
    verifiedCheckBox = new wxCheckBox(this, wxID_ANY, "Show verified Tracks");
    verifiedCheckBox->SetValue(true);

    subtreeCheckBox =
        new wxCheckBox(this, wxID_ANY, "Show Tracks in subfolders");
    subtreeCheckBox->SetValue(false);
    itemCount = new wxStaticText(this, wxID_ANY, "");

    searchBar = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition,
                               wxDefaultSize, wxTE_PROCESS_ENTER);
    searchBar->Bind(wxEVT_TEXT_ENTER, &LocalTrackWindow::OnSearch, this);
    searchBar->Bind(wxEVT_TEXT, [this](wxCommandEvent &) {
        searchDebounceTimer.StartOnce(500); // 200ms debounce
    });
    searchDebounceTimer.Bind(wxEVT_TIMER, [this](wxTimerEvent &) { search(); });

    toolbarSizer->Add(unverifiedCheckBox, 0, wxALL, 5);
    toolbarSizer->Add(verifiedCheckBox, 0, wxALL, 5);
    toolbarSizer->Add(subtreeCheckBox, 0, wxALL, 5);
    toolbarSizer->Add(itemCount, 0, wxALL, 5);
    toolbarSizer->Add(searchBar, 1, wxALL, 5);

    trackWindow = new TrackWindow(this, _downloader);
    trackEditWindow = new TrackEditWindow(this);
    trackEditWindow->Hide();

    auto trackSizer = new wxBoxSizer(wxHORIZONTAL);
    trackSizer->Add(trackWindow, 1, wxEXPAND | wxALL, 5);
    trackSizer->Add(trackEditWindow, 0, wxSHRINK | wxALL, 5);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(trackSizer, 1, wxEXPAND | wxALL, 5);

    Bind(EVT_TRACKWINDOW_SCROLL_BOTTOM, &LocalTrackWindow::OnScrollEnd, this);
    Bind(EVT_TRACKLABEL_CLICKED, [this](wxCommandEvent &_event) {
        auto *label = static_cast<TrackLabel *>(_event.GetEventObject());

        if (!label) {
            wxLogMessage("Invalid TrackLabel event source");
            return;
        }
        const bool multi = _event.GetInt();
        trackEditWindow->toggleSelection(label, multi);
    });

    unverifiedCheckBox->Bind(
        wxEVT_CHECKBOX, [this](wxCommandEvent &event) { this->refresh(); });
    verifiedCheckBox->Bind(wxEVT_CHECKBOX,
                           [this](wxCommandEvent &event) { this->refresh(); });
    subtreeCheckBox->Bind(wxEVT_CHECKBOX,
                          [this](wxCommandEvent &event) { this->refresh(); });

    size_t currentGen = trackService.getGeneration();
    trackService.onBatch =
        [this, currentGen](std::vector<std::shared_ptr<TrackInterface>> _batch,
                           size_t _gen) {
            wxTheApp->CallAfter(
                [this, batch = std::move(_batch), _gen]() mutable {
                    if (_gen != trackService.getGeneration()) {
                        wxLogError(wxT("Outdated generation!"));
                        return; // stale batch
                    }

                    for (auto &track : batch) {
                        allTracks.push_back(track);
                    }

                    // Re-apply current filter incrementally
                    applyFilterIncremental(batch);

                    // Optional: initial fill
                    if (loadedCount == 0) {
                        LoadMoreItems(LOAD_CHUNK);
                    }

                    OnTracksUpdated();
                    Layout();
                });
        };

    refresh();

    SetSizerAndFit(mainSizer);
}

LocalTrackWindow::~LocalTrackWindow() {}

void LocalTrackWindow::refresh() {

    searchBar->Clear();
    loadedCount = 0;
    allTracks.clear();
    filteredIndices.clear();

    trackWindow->Clear();
    trackEditWindow->Hide();

    std::filesystem::path musicPath =
        trackWindow->get_downloader()->get_trackPath();
    trackService.loadTracks(
        musicPath, trackWindow->get_downloader(), subtreeCheckBox->GetValue(),
        verifiedCheckBox->GetValue(), unverifiedCheckBox->GetValue());

    if (!std::filesystem::exists(musicPath) ||
        !std::filesystem::is_directory(musicPath)) {
        wxMessageBox("Music directory not found!", "Error", wxICON_ERROR);
        return;
    }
}

void LocalTrackWindow::LoadMoreItems(std::size_t _count) {
    size_t added = 0;

    while (added < _count && loadedCount < filteredIndices.size()) {
        size_t idx = filteredIndices[loadedCount];

        auto &track = allTracks[idx];
        trackWindow->appendChild(track);

        ++loadedCount;
        ++added;
    }

    loadedCount += added;
    OnTracksUpdated();
    trackWindow->Layout();
    trackWindow->FitInside();
}

void LocalTrackWindow::OnScrollEnd(wxCommandEvent &_event) {
    _event.Skip();
    LoadMoreItems(LOAD_CHUNK);
}

void LocalTrackWindow::search() {
    filteredIndices.clear();
    trackWindow->Clear();
    loadedCount = 0;

    currentQuery = searchBar->GetValue().ToStdString();

    size_t myGen = ++searchGeneration;

    // Snapshot UI state (safe, UI thread)
    auto snapshot = allTracks;
    auto query = currentQuery;

    std::thread([this, snapshot = std::move(snapshot), query, myGen]() mutable {
        std::vector<size_t> result;
        result.reserve(snapshot.size());

        for (size_t i = 0; i < snapshot.size(); ++i) {
            if (matchesSearch(snapshot[i], query)) {
                result.push_back(i);
            }
        }

        // Return to UI thread
        wxTheApp->CallAfter(
            [this, result = std::move(result), myGen]() mutable {
                if (myGen != searchGeneration)
                    return; // stale result

                filteredIndices = std::move(result);
                LoadMoreItems(LOAD_CHUNK);
            });
    }).detach();
}

void LocalTrackWindow::OnSearch(wxCommandEvent &) { search(); }

void LocalTrackWindow::OnTracksUpdated() {
    itemCount->SetLabelText("Showing " + std::to_string(loadedCount) + "/" +
                            std::to_string(allTracks.size()) + " items");
}

void LocalTrackWindow::applyFilterIncremental(
    const std::vector<std::shared_ptr<TrackInterface>> &batch) {
    size_t baseIndex = allTracks.size() - batch.size();

    for (size_t i = 0; i < batch.size(); ++i) {
        if (matchesSearch(batch[i], searchBar->GetValue().ToStdString())) {
            filteredIndices.push_back(baseIndex + i);
        }
    }
}

std::string LocalTrackWindow::toLower(std::string _s) {
    std::transform(_s.begin(), _s.end(), _s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return _s;
};

bool LocalTrackWindow::matchesSearch(std::shared_ptr<TrackInterface> _track,
                                     const std::string &_query) {

    if (_query.empty())
        return true;

    // Normalize query
    std::string query = toLower(_query);

    // --- Filename ---
    std::string filename = toLower(_track->get_id()); // assume exists

    if (filename.find(query) != std::string::npos)
        return true;

    auto contains = [&](const std::string &_str) {
        return toLower(_str).find(query) != std::string::npos;
    };

    return contains(_track->get_title()) || contains(_track->get_artist()) ||
           contains(_track->get_album()) || contains(_track->get_genre());
}