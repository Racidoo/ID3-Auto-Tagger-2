#include "../include/LocalTrackWindow.h"
#include "../include/Downloader.h"
#include "../include/TrackEditWindow.h"
#include "../include/TrackLabel.h"

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
    searchDebounceTimer.Bind(wxEVT_TIMER, &LocalTrackWindow::OnSearch, this);

    toolbarSizer->Add(unverifiedCheckBox, 0, wxALL, 5);
    toolbarSizer->Add(verifiedCheckBox, 0, wxALL, 5);
    toolbarSizer->Add(subtreeCheckBox, 0, wxALL, 5);
    toolbarSizer->Add(itemCount, 0, wxALL, 5);
    toolbarSizer->Add(searchBar, 1, wxALL, 5);

    trackPanel = new TrackPanel(this);
    trackWindow = new TrackWindow(this, _downloader);
    trackWindow->Hide();
    trackEditWindow = new TrackEditWindow(this);
    trackEditWindow->Hide();

    auto trackSizer = new wxBoxSizer(wxHORIZONTAL);
    trackSizer->Add(trackPanel, 1, wxEXPAND | wxALL, 5);
    trackSizer->Add(trackEditWindow, 0, wxSHRINK | wxALL, 5);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(trackSizer, 1, wxEXPAND | wxALL, 5);

    unverifiedCheckBox->Bind(wxEVT_CHECKBOX, &LocalTrackWindow::OnSearch, this);
    verifiedCheckBox->Bind(wxEVT_CHECKBOX, &LocalTrackWindow::OnSearch, this);
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
                    trackPanel->MergeTracks(batch);
                });
        };

    refresh();

    SetSizerAndFit(mainSizer);
}

LocalTrackWindow::~LocalTrackWindow() {}

void LocalTrackWindow::refresh() {

    searchBar->Clear();
    // substitution for Clear()
    trackPanel->MergeTracks({});
    trackEditWindow->Hide();

    std::filesystem::path musicPath =
        trackWindow->get_downloader()->get_trackPath();
    trackService.loadTracks(musicPath, trackWindow->get_downloader(),
                            subtreeCheckBox->GetValue());

    if (!std::filesystem::exists(musicPath) ||
        !std::filesystem::is_directory(musicPath)) {
        wxMessageBox("Music directory not found!", "Error", wxICON_ERROR);
        return;
    }
}

void LocalTrackWindow::OnSearch(wxEvent &) {
    trackPanel->Search(searchBar->GetValue(), verifiedCheckBox->GetValue(),
                       unverifiedCheckBox->GetValue());
}
