#if !defined(TRACK_WINDOW_H)
#define TRACK_WINDOW_H

#include <string>
#include <unordered_map>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/wx.h>

#include "TrackInterface.h"

wxDECLARE_EVENT(EVT_SHOW_TRACK_DETAILS, wxCommandEvent);
wxDECLARE_EVENT(EVT_TRACKWINDOW_SCROLL_BOTTOM, wxCommandEvent);

class Downloader;
class TrackLabel;

class TrackWindow : public wxScrolledWindow {
  protected:
    wxPanel *content;
    wxBoxSizer *contentSizer;

    std::unordered_map<std::string, TrackLabel *> trackLabels;
    Downloader *downloader;

  public:
    TrackWindow(wxWindow *_parent, Downloader *_downloader);
    ~TrackWindow() override = default;

    inline wxBoxSizer *GetContentSizer() { return contentSizer; };
    inline wxPanel *GetContentPanel() { return content; }

    // Add a new TrackLabel safely
    void appendChild(std::shared_ptr<TrackInterface::TrackViewData> data);

    // Filter existing labels by query
    void filterTracks(const std::string &query);

    void Clear();

    inline std::unordered_map<std::string, TrackLabel *> &get_trackLabels() {
        return trackLabels;
    }
    inline const std::unordered_map<std::string, TrackLabel *> &
    get_trackLabels() const {
        return trackLabels;
    }

    inline Downloader *get_downloader() const { return downloader; }

    void createHeader();
    void sortByHeader();

    TrackLabel *getLabel(const std::string &_id);

    void deleteChild(TrackLabel *_trackLabel);
    void OnScroll(wxScrollWinEvent &event);
    // void processClickedLabel(wxCommandEvent &event);
};

#endif // TRACK_WINDOW_H
