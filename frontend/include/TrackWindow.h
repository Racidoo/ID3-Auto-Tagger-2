#if !defined(TRACK_WINDOW_H)
#define TRACK_WINDOW_H

#include <set>
#include <string>
#include <unordered_map>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/wx.h>

#include "TrackLabel.h"

wxDECLARE_EVENT(EVT_SHOW_TRACK_DETAILS, wxCommandEvent);

class TrackWindow : public wxScrolledWindow {
  private:
    std::unordered_map<std::string, TrackLabel *> trackLabels;
    wxBoxSizer *mainSizer;
    std::set<TrackLabel *> activeSongs;

  public:
    TrackWindow(wxWindow *_parent);
    ~TrackWindow();

    inline std::unordered_map<std::string, TrackLabel *>
    get_trackLabels() const {
        return trackLabels;
    }
    inline std::set<TrackLabel *> get_activeSongs() const {
        return activeSongs;
    }

    void deleteChildren();
    void appendChildren(TrackLabel *_trackLabel);
    void onScroll(wxScrollWinEvent &event);
    void processClickedLabel(wxCommandEvent &event);
};

#endif // TRACK_WINDOW_H
