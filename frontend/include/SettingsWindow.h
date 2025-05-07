#if !defined(SETTINGS_WINDOW_H)
#define SETTINGS_WINDOW_H

#include <filesystem>
#include <wx/wx.h>

class Downloader;

class SettingsWindow : public wxScrolledWindow {
  public:
    SettingsWindow(wxWindow *_parent, Downloader *_downloader,
                   wxWindowID _winid = wxID_ANY,
                   const wxPoint &_pos = wxDefaultPosition,
                   const wxSize &_size = wxDefaultSize);
    ~SettingsWindow();

  private:
    Downloader *downloader;
};

#endif // SETTINGS_WINDOW_H
