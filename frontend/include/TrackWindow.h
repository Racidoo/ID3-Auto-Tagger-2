#if !defined(TRACK_WINDOW_H)
#define TRACK_WINDOW_H

#include <unordered_map>
#include <string>
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
};

#endif // TRACK_WINDOW_H
