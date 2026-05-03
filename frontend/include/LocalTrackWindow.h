#if !defined(LOCAL_TRACK_WINDOW_H)
#define LOCAL_TRACK_WINDOW_H

#include <filesystem>
#include <wx/event.h>
#include <wx/wx.h>

#include "LocalTrackService.h"
#include "TrackInterface.h"
#include "TrackPanel.h"

class TrackEditWindow;
class Downloader;

class LocalTrackWindow : public wxScrolledWindow {
  private:
    TrackPanel *trackPanel;
    TrackEditWindow *trackEditWindow;
    wxStaticText *itemCount;
    wxCheckBox *unverifiedCheckBox;
    wxCheckBox *verifiedCheckBox;
    wxCheckBox *subtreeCheckBox;

    wxTextCtrl *searchBar;

    LocalTrackService trackService;

    wxTimer searchDebounceTimer;

  public:
    LocalTrackWindow(wxWindow *_parent, Downloader *_downloader);
    ~LocalTrackWindow();

    void refresh();
    void OnSearch(wxEvent &_event);
};

#endif // LOCAL_TRACK_WINDOW_H
