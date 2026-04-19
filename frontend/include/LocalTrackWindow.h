#if !defined(LOCAL_TRACK_WINDOW_H)
#define LOCAL_TRACK_WINDOW_H

#include <filesystem>
#include <wx/event.h>
#include <wx/wx.h>

#include "LocalTrackService.h"
#include "TrackInterface.h"

class TrackWindow;
class TrackEditWindow;
class Downloader;

class LocalTrackWindow : public wxScrolledWindow {
  private:
    TrackWindow *trackWindow;
    TrackEditWindow * trackEditWindow;
    wxStaticText *itemCount;
    wxCheckBox *unverifiedCheckBox;
    wxCheckBox *verifiedCheckBox;
    wxCheckBox *subtreeCheckBox;

    wxTextCtrl *searchBar;

    LocalTrackService trackService;

    std::size_t loadedCount;
    constexpr static std::size_t LOAD_CHUNK = 32;

    void search();
    void OnTracksUpdated();

  public:
    LocalTrackWindow(wxWindow *_parent, Downloader *_downloader);
    ~LocalTrackWindow();

    inline TrackWindow *get_trackWindow() const { return trackWindow; }

    void refresh();
    void LoadMoreItems(std::size_t _count);
    void OnScrollEnd(wxCommandEvent &_event);
    void OnSearch(wxCommandEvent &_event);
};

#endif // LOCAL_TRACK_WINDOW_H
