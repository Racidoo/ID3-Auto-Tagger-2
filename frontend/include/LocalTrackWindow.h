#if !defined(LOCAL_TRACK_WINDOW_H)
#define LOCAL_TRACK_WINDOW_H

#include <filesystem>
#include <wx/event.h>
#include <wx/wx.h>

#include "LocalTrackService.h"
#include "TrackInterface.h"
#include "TrackPanel.h"
#include "TrackWindow.h"

class TrackEditWindow;
class Downloader;

class LocalTrackWindow : public wxScrolledWindow {
  private:
    TrackWindow *trackWindow;
    TrackPanel *trackPanel;
    TrackEditWindow *trackEditWindow;
    wxStaticText *itemCount;
    wxCheckBox *unverifiedCheckBox;
    wxCheckBox *verifiedCheckBox;
    wxCheckBox *subtreeCheckBox;

    wxTextCtrl *searchBar;

    LocalTrackService trackService;
    std::deque<std::shared_ptr<TrackInterface>> pendingTracks;

    std::vector<std::shared_ptr<TrackInterface>> allTracks;
    std::vector<size_t> filteredIndices;

    std::size_t loadedCount;
    constexpr static std::size_t LOAD_CHUNK = 32;

    std::string currentQuery;
    size_t searchGeneration = 0;

    wxTimer searchDebounceTimer;

    TrackWindow::SortKey currentSortKey = TrackWindow::SortKey::Title;
    bool ascending = true;
    bool sortDirty = true;

    // void search();
    // void OnTracksUpdated();

    // void applyFilterIncremental(
        // const std::vector<std::shared_ptr<TrackInterface>> &batch);
    static std::string toLower(std::string _s);
    static bool matchesSearch(std::shared_ptr<TrackInterface> _track,
                              const std::string &_query);
    // void applySortAndRefresh();
    // void sortFiltered();
    // void ensureSorted();

  public:
    LocalTrackWindow(wxWindow *_parent, Downloader *_downloader);
    ~LocalTrackWindow();

    // inline TrackWindow *get_trackWindow() const { return trackWindow; }

    void refresh();
    // void LoadMoreItems(std::size_t _count);
    // void OnScrollEnd(wxCommandEvent &_event);
    void OnSearch(wxCommandEvent &_event);
};

#endif // LOCAL_TRACK_WINDOW_H
