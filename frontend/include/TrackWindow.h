#if !defined(TRACK_WINDOW_H)
#define TRACK_WINDOW_H

#include <set>
#include <string>
#include <unordered_map>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/wx.h>

wxDECLARE_EVENT(EVT_SHOW_TRACK_DETAILS, wxCommandEvent);
class Downloader;
class TrackLabel;

class TrackWindow : public wxScrolledWindow {
  private:
    std::unordered_map<std::string, TrackLabel *> trackLabels;
    Downloader *downloader;
    std::set<TrackLabel *> activeSongs;

  public:
    TrackWindow(wxWindow *_parent, Downloader* _downloader);
    ~TrackWindow();

    inline std::unordered_map<std::string, TrackLabel *>
    get_trackLabels() const {
        return trackLabels;
    }
    inline std::set<TrackLabel *> get_activeSongs() const {
        return activeSongs;
    }

    void createHeader();
    void sortByHeader();

    void deleteChildren();
    void appendChildren(TrackLabel *_trackLabel);
    void onScroll(wxScrollWinEvent &event);
    void processClickedLabel(wxCommandEvent &event);
};

#endif // TRACK_WINDOW_H
