#include "../include/LabeledTextCtrl.h"

wxDEFINE_EVENT(EVT_VALUE_CHANGE, wxCommandEvent);

LabeledTextCtrl::LabeledTextCtrl(wxWindow *_parent, wxWindowID _winid,
                                 const wxString &_labelText)
    : wxPanel(_parent, _winid) {
    auto *sizer = new wxBoxSizer(wxVERTICAL);
    label = new wxStaticText(this, wxID_ANY, _labelText);
    textCtrl = new wxTextCtrl(this, wxID_ANY);
    textCtrl->SetHint(_labelText);
    sizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    sizer->Add(textCtrl, 1, wxEXPAND);
    SetSizerAndFit(sizer);

    label->Bind(wxEVT_TEXT_ENTER, &LabeledTextCtrl::onValueChange, this);
    textCtrl->Bind(wxEVT_TEXT_ENTER, &LabeledTextCtrl::onValueChange, this);
    this->Bind(wxEVT_TEXT_ENTER, &LabeledTextCtrl::onValueChange, this);
}

void LabeledTextCtrl::SetValue(const wxString &_value) {
    if (_value.empty()) {
        textCtrl->Clear();
        textCtrl->SetHint(label->GetLabelText());
        return;
    }
    textCtrl->SetValue(_value);
}

void LabeledTextCtrl::onValueChange(wxCommandEvent &_event) {
    wxLogDebug(wxT("Sent event: ") + textCtrl->GetValue());
    wxCommandEvent event(EVT_VALUE_CHANGE);
    event.SetString(textCtrl->GetValue());
    wxPostEvent(GetParent(), event);
}