#include "../include/LabeledTextCtrl.h"

wxDEFINE_EVENT(EVT_VALUE_CHANGE, wxCommandEvent);

LabeledTextCtrl::LabeledTextCtrl(wxWindow *_parent, wxWindowID _winid,
                                 Spotify::Track::tag_type_t _type,
                                 const wxString &_labelText)
    : wxPanel(_parent, _winid), type(_type) {
    auto *sizer = new wxBoxSizer(wxVERTICAL);
    label = new wxStaticText(this, wxID_ANY, _labelText);
    textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, wxTE_PROCESS_ENTER);
    textCtrl->SetHint(_labelText);
    sizer->Add(label, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT, 5);
    sizer->Add(textCtrl, 1, wxEXPAND);
    SetSizerAndFit(sizer);

    // label->Bind(wxEVT_TEXT_ENTER, &LabeledTextCtrl::onValueChange, this);
    textCtrl->Bind(wxEVT_TEXT_ENTER, &LabeledTextCtrl::onValueChange, this);
    // this->Bind(wxEVT_TEXT_ENTER, &LabeledTextCtrl::onValueChange, this);
}

void LabeledTextCtrl::onValueChange(wxCommandEvent &_event) {
    wxLogDebug(wxT("Sent event: ") + textCtrl->GetValue());
    wxCommandEvent event(EVT_VALUE_CHANGE);
    event.SetString(textCtrl->GetValue());
    event.SetInt(type);
    wxPostEvent(GetParent(), event);
}

void LabeledTextCtrl::SetValue(const wxString &_value) {
    if (_value.empty()) {
        // Known Bug: Hint is only set on second call
        textCtrl->Clear();
        textCtrl->SetHint(label->GetLabelText());
        return;
    }
    textCtrl->SetValue(_value);
}