#include "../include/LocalTrackWindow.h"
#include "../include/Downloader.h"
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

    toolbarSizer->Add(unverifiedCheckBox, 0, wxALL, 5);
    toolbarSizer->Add(verifiedCheckBox, 0, wxALL, 5);
    toolbarSizer->Add(subtreeCheckBox, 0, wxALL, 5);
    toolbarSizer->Add(itemCount, 0, wxALL, 5);
    toolbarSizer->Add(searchBar, 1, wxALL, 5);

    trackWindow = new TrackWindow(this, _downloader);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(trackWindow, 1, wxEXPAND | wxALL, 5);

    Bind(EVT_TRACKWINDOW_SCROLL_BOTTOM, &LocalTrackWindow::OnScrollEnd, this);
    // Bind(EVT_TRACKS_UPDATED, &LocalTrackWindow::OnTracksUpdated, this);
    trackService.setCallback(
        [this]() { CallAfter([this]() { OnTracksUpdated(); }); });

    unverifiedCheckBox->Bind(
        wxEVT_CHECKBOX, [this](wxCommandEvent &event) { this->refresh(); });
    verifiedCheckBox->Bind(wxEVT_CHECKBOX,
                           [this](wxCommandEvent &event) { this->refresh(); });
    subtreeCheckBox->Bind(wxEVT_CHECKBOX,
                          [this](wxCommandEvent &event) { this->refresh(); });

    refresh();

    SetSizerAndFit(mainSizer);
}

LocalTrackWindow::~LocalTrackWindow() {}

void LocalTrackWindow::refresh() {
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

    CallAfter([this]() {
        loadedCount = 0;
        LoadMoreItems(LOAD_CHUNK);
        trackWindow->Show();
        trackWindow->Layout();
        Layout();
    });
}

void LocalTrackWindow::LoadMoreItems(std::size_t _count) {
    auto nextIndices = trackService.getNextChunk(_count);

    for (auto idx : nextIndices) {
        trackWindow->appendChild(trackService.getTrack(idx));
    }

    loadedCount += nextIndices.size();
    OnTracksUpdated();
    trackWindow->Layout();
    trackWindow->FitInside();
}

void LocalTrackWindow::OnScrollEnd(wxCommandEvent &_event) {
    _event.Skip();
    LoadMoreItems(LOAD_CHUNK);
}

void LocalTrackWindow::search() {
    trackService.setSearchQuery(searchBar->GetValue().ToStdString());
    trackWindow->Clear();
    loadedCount = 0;
    LoadMoreItems(LOAD_CHUNK);
}

void LocalTrackWindow::OnSearch(wxCommandEvent &) { search(); }

void LocalTrackWindow::OnTracksUpdated() {
    itemCount->SetLabelText("Showing " + std::to_string(loadedCount) + "/" +
                            std::to_string(trackService.getFilteredCount()) +
                            " items");
}
