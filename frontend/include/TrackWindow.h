#if !defined(TRACK_WINDOW_H)
#define TRACK_WINDOW_H

#include <cctype>
#include <string>
#include <unordered_map>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/wx.h>

#include "TrackInterface.h"

wxDECLARE_EVENT(EVT_SHOW_TRACK_DETAILS, wxCommandEvent);
wxDECLARE_EVENT(EVT_TRACKWINDOW_SCROLL_BOTTOM, wxCommandEvent);
wxDECLARE_EVENT(EVT_SORT_CHANGED, wxCommandEvent);

class Downloader;
class TrackLabel;

class TrackWindow : public wxPanel {
  public:
    enum class SortKey { Title, Artist, Album, Genre, Length };

  protected:
    wxPanel *headerPanel;
    wxScrolledWindow *contentPanel;
    wxBoxSizer *contentSizer;

    std::unordered_map<std::string, TrackLabel *> trackLabels;
    Downloader *downloader;

    // SortKey currentSortKey = SortKey::Title;
    // bool ascending = true;

    void createHeader();
    void sendSortEvent(SortKey _key);
    // void refreshLayout();
    // std::vector<TrackLabel *> getSortedLabels(SortKey _key);

  public:
    TrackWindow(wxWindow *_parent, Downloader *_downloader);
    ~TrackWindow() override = default;

    inline wxBoxSizer *GetContentSizer() { return contentSizer; };
    inline wxPanel *GetContentPanel() { return contentPanel; }

    // Add a new TrackLabel safely
    void appendChild(std::shared_ptr<TrackInterface> data);

    void Clear();

    inline std::unordered_map<std::string, TrackLabel *> &get_trackLabels() {
        return trackLabels;
    }
    inline const std::unordered_map<std::string, TrackLabel *> &
    get_trackLabels() const {
        return trackLabels;
    }

    inline Downloader *get_downloader() const { return downloader; }

    TrackLabel *getLabel(const std::string &_id);

    void deleteChild(TrackLabel *_trackLabel);
    void OnScroll(wxScrollWinEvent &event);
};

#endif // TRACK_WINDOW_H
