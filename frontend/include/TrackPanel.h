#pragma once

#include <memory>
#include <unordered_map>
#include <wx/dataview.h>
#include <wx/wx.h>

class TrackInterface;
class TrackModel;
class Downloader;

class TrackPanel : public wxPanel {
  public:
    explicit TrackPanel(wxWindow *parent, Downloader *_downloader);

    Downloader *get_downloader() const;

    void Refresh();
    void
    MergeTracks(const std::vector<std::shared_ptr<TrackInterface>> &_batch);

    void Search(const wxString &_query, bool _showVerified,
                bool _showUnverified);

  private:
    wxDataViewCtrl *ctrl{};
    TrackModel *model{};
    Downloader *downloader;

    void OnSelectionChanged(wxDataViewEvent &event);
    void OnActivated(wxDataViewEvent &event);
    void OnColumnSorted(wxDataViewEvent &_event);
    void OnLeftDown(wxMouseEvent &event);
};