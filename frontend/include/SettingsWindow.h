#if !defined(SETTINGS_WINDOW_H)
#define SETTINGS_WINDOW_H

#include <wx/wx.h>

class SettingsWindow : public wxScrolledWindow {
  public:
    SettingsWindow(wxWindow *_parent, wxWindowID _winid = wxID_ANY,
                   const wxPoint &_pos = wxDefaultPosition,
                   const wxSize &_size = wxDefaultSize);
    ~SettingsWindow();

  private:
    wxBoxSizer *mainSizer;
};

#endif // SETTINGS_WINDOW_H
