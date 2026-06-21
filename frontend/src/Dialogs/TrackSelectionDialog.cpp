#include "Dialogs/TrackSelectionDialog.h"
#include "Media/Track/ComparisonTrackPanel.h"

TrackSelectionDialog::TrackSelectionDialog(
    wxWindow *_parent, const TagService::TagResearchResult &_result)
    : wxDialog(_parent, wxID_ANY, "Choose a source track", wxDefaultPosition,
               wxSize(800, 600), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {

    auto contentSizer = new wxBoxSizer(wxVERTICAL);

    std::vector<std::shared_ptr<ITrack>> candidates;
    for (auto &&candidate : _result.candidates) {
        candidates.push_back(candidate.track);
    }

    trackPanel = new ComparisonTrackPanel(this);
    contentSizer->Add(trackPanel, 1, wxEXPAND | wxALL, 5);
    // trackPanel->MergeTracks(_result.candidates);
    trackPanel->MergeTracks(candidates);

    auto *buttonSizer = new wxStdDialogButtonSizer();
    auto *okButton = new wxButton(this, wxID_OK);
    auto *cancelButton = new wxButton(this, wxID_CANCEL);

    okButton->Enable(false);

    buttonSizer->AddButton(cancelButton);
    buttonSizer->AddButton(okButton);
    buttonSizer->Realize();

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(contentSizer, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 5);

    Bind(EVT_TRACK_SELECTION_CHANGED,
         [okButton](wxCommandEvent &event) { okButton->Enable(); });

    SetSizer(mainSizer);
    Layout();
    SetSize(800, 600);
    Centre();
}

std::shared_ptr<ITrack> TrackSelectionDialog::getSelectedTrack() {
    auto rows = trackPanel->GetSelectedRows();
    if (rows.size() != 1) {
        return nullptr;
    }
    return trackPanel->GetTrack(rows[0]);
}
