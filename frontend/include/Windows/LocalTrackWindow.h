#if !defined(LOCAL_TRACK_WINDOW_H)
#define LOCAL_TRACK_WINDOW_H

#include <filesystem>
#include <wx/event.h>
#include <wx/wx.h>

class LocalTrackPanel;
class TrackEditWindow;
class LocalTrackService;
class SearchService;

class LocalTrackWindow : public wxScrolledWindow {
  private:
    LocalTrackPanel *trackPanel;
    TrackEditWindow *trackEditWindow;
    wxStaticText *itemCount;
    wxCheckBox *unverifiedCheckBox;
    wxCheckBox *verifiedCheckBox;
    wxCheckBox *subtreeCheckBox;

    wxTextCtrl *searchBar;

    LocalTrackService *trackService;

    wxTimer searchDebounceTimer;

  public:
    LocalTrackWindow(wxWindow *_parent, LocalTrackService *_trackService,
                     SearchService *_searchService);
    ~LocalTrackWindow();

    void refresh();
    void OnSearch(wxEvent &_event);
};

#endif // LOCAL_TRACK_WINDOW_H
