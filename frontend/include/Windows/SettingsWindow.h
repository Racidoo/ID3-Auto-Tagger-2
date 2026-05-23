#if !defined(SETTINGS_WINDOW_H)
#define SETTINGS_WINDOW_H

#include <filesystem>
#include <wx/wx.h>

#include "Services/MediaServiceRegistry.h"

class SettingsWindow : public wxScrolledWindow {
  public:
    SettingsWindow(wxWindow *_parent, MediaServiceRegistry *_registry,
                   wxWindowID _winid = wxID_ANY,
                   const wxPoint &_pos = wxDefaultPosition,
                   const wxSize &_size = wxDefaultSize);
    ~SettingsWindow() = default;
};

#endif // SETTINGS_WINDOW_H
