#if !defined(MULTI_INPUT_DIALOG_H)
#define MULTI_INPUT_DIALOG_H

#include <unordered_map>
#include <vector>
#include <tuple>
#include <wx/wx.h>

class MultiInputDialog : public wxDialog {
  public:
    using FieldSpec = std::tuple<std::string, wxString, bool>;
    // Pass a list of (label, isPassword) pairs
    MultiInputDialog(wxWindow *_parent, const wxString &_title,
        const std::vector<FieldSpec>& _fields);

    std::unordered_map<std::string, wxString> GetValues() const;

  private:
    std::unordered_map<std::string, wxTextCtrl *> _inputs;
};
#endif // MULTI_INPUT_DIALOG_H
