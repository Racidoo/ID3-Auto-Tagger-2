#if !defined(TRACK_WINDOW_H)
#define TRACK_WINDOW_H

#include <string>
#include <unordered_map>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/wx.h>

#include "TrackLabel.h"

class TrackWindow : public wxScrolledWindow {
  private:
    std::unordered_map<std::string, TrackLabel *> trackLabels;
    wxBoxSizer *mainSizer;

  public:
    TrackWindow(wxWindow *_parent);
    ~TrackWindow();

    std::unordered_map<std::string, TrackLabel *> get_trackLabels() const {
        return trackLabels;
    }

    void deleteChildren();
    void appendChildren(TrackLabel *_trackLabel);
    void onScroll(wxScrollWinEvent &event);
};

#endif // TRACK_WINDOW_H
