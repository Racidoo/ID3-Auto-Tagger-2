#include "../include/MultiInputDialog.h"

MultiInputDialog::MultiInputDialog(wxWindow *_parent, const wxString &_title,
                                   const std::vector<FieldSpec> &_fields)
    : wxDialog(_parent, wxID_ANY, _title, wxDefaultPosition, wxDefaultSize) {

    wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

    for (const auto &[key, label, isPassword] : _fields) {
        wxBoxSizer *columnSizer = new wxBoxSizer(wxVERTICAL);
        columnSizer->Add(new wxStaticText(this, wxID_ANY, label), 0,
                         wxALIGN_LEFT | wxALL, 5);

        long style = isPassword ? wxTE_PASSWORD : 0;
        wxTextCtrl *ctrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition,
                                          wxDefaultSize, style);
        columnSizer->Add(ctrl, 1, wxEXPAND | wxALL, 5);
        topsizer->Add(columnSizer, 0, wxEXPAND | wxALL, 5);

        _inputs[key] = ctrl;
    }

    topsizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0,
                  wxEXPAND | wxALL, 10);

    SetSizerAndFit(topsizer);
    Centre();
}

// Retrieve input values by label
std::unordered_map<std::string, wxString> MultiInputDialog::GetValues() const {
    std::unordered_map<std::string, wxString> result;
    for (const auto &[key, ctrl] : _inputs) {
        result[key] = ctrl->GetValue();
    }
    return result;
}