#include "Dialogs/VideoSelectionDialog.h"
#include "Media/Video/VideoPanel.h"

VideoSelectionDialog::VideoSelectionDialog(
    wxWindow *_parent, const std::vector<std::shared_ptr<IVideo>> &_result)
    : wxDialog(_parent, wxID_ANY, "Choose a source", wxDefaultPosition,
               wxSize(800, 600), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {

    auto contentSizer = new wxBoxSizer(wxVERTICAL);

    videoPanel = new VideoPanel(this);
    contentSizer->Add(videoPanel, 1, wxEXPAND | wxALL, 5);

    videoPanel->Merge(_result);

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

    Bind(EVT_VIDEO_SELECTION_CHANGED,
         [okButton](wxCommandEvent &event) { okButton->Enable(); });

    SetSizer(mainSizer);
    Layout();
    SetSize(800, 600);
    Centre();
}

std::shared_ptr<IVideo> VideoSelectionDialog::getSelectedVideo() {
    auto rows = videoPanel->GetSelectedRows();
    if (rows.size() != 1) {
        return nullptr;
    }
    return videoPanel->GetVideo(rows[0]);
}
