#include "Windows/LocalTrackWindow.h"
#include "Components/LabeledTextCtrl.h"
#include "Local/LocalTrackService.h"
#include "Media/Track/LocalTrackPanel.h"
#include "Services/SearchService.h"
#include "Services/TagService.h"
#include "Windows/TrackEditWindow.h"

LocalTrackWindow::LocalTrackWindow(wxWindow *_parent,
                                   LocalTrackService *_trackService,
                                   SearchService *_searchService)
    : wxScrolledWindow(_parent, wxID_ANY), trackService(_trackService) {

    if (!_trackService) {
        wxLogInfo(wxT("Local service is currently unavailable!"));
        return;
    }

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

    trackPanel = new LocalTrackPanel(this);
    trackEditWindow = new TrackEditWindow(this);

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
    Bind(EVT_TRACK_SELECTION_CHANGED, [this](wxCommandEvent &) {
        trackEditWindow->set_selected(trackPanel->GetTracksOfSelectedRows());
        trackEditWindow->Update();
        trackEditWindow->Show();
        Layout();
    });

    this->Bind(EVT_VALUE_CHANGE, [this](wxCommandEvent &event) {
        std::string value = event.GetString().ToStdString();
        LocalTrack::tag_type_t type =
            static_cast<LocalTrack::tag_type_t>(event.GetInt());
        trackPanel->ApplyChangeToSelectedRows(type, value);
    });

    this->Bind(EVT_TRACK_VERIFY, [this, _searchService](wxCommandEvent &event) {
        auto rows = trackPanel->GetSelectedRows();

        for (auto row : rows) {
            auto track = std::dynamic_pointer_cast<LocalTrack>(
                trackPanel->GetTrack(row));
            auto id = TagService::researchMissingTags(track, _searchService);
            if (!id.empty()) {
                trackService->renameTrack(track, id);
                trackService->makeBlocked(track);
                trackPanel->UpdateRow(row);
            }
        }
    });

    size_t currentGen = trackService->getGeneration();
    trackService->onBatch = [this, currentGen](
                                std::vector<std::shared_ptr<ITrack>> _batch,
                                size_t _gen) {
        wxTheApp->CallAfter([this, batch = std::move(_batch), _gen]() mutable {
            if (_gen != trackService->getGeneration()) {
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

    trackService->loadTracks(subtreeCheckBox->GetValue());

    // if (!std::filesystem::exists(musicPath) ||
    //     !std::filesystem::is_directory(musicPath)) {
    //     wxMessageBox("Music directory not found!", "Error",
    //     wxICON_ERROR); return;
    // }
}

void LocalTrackWindow::OnSearch(wxEvent &) {
    trackPanel->Search(searchBar->GetValue(), verifiedCheckBox->GetValue(),
                       unverifiedCheckBox->GetValue());
}
