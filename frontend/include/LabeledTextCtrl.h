#if !defined(LABELED_TEXT_CTRL_H)
#define LABELED_TEXT_CTRL_H

#include "Spotify/Track.h" // tag_type_t
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/wx.h>

wxDECLARE_EVENT(EVT_VALUE_CHANGE, wxCommandEvent);

class LabeledTextCtrl : public wxPanel {
  public:
    LabeledTextCtrl(wxWindow *_parent, wxWindowID _winid,
                    Spotify::Track::tag_type_t _type,
                    const wxString &_labelText = wxEmptyString);

    inline wxString GetValue() const { return textCtrl->GetValue(); }

    inline void SetHint(const wxString &_hint) { textCtrl->SetHint(_hint); }

    void SetValue(const wxString &_value);

  private:
    wxStaticText *label;
    wxTextCtrl *textCtrl;
    Spotify::Track::tag_type_t type;
    void onValueChange(wxCommandEvent &_event);
};

#endif // LABELED_TEXT_CTRL_H
