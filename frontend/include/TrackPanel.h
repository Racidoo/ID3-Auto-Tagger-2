#pragma once

#include <memory>
#include <unordered_map>
#include <wx/dataview.h>
#include <wx/wx.h>

#include "LocalTrack.h" // tag_type_t

class TrackInterface;
class TrackModel;
class Downloader;

wxDECLARE_EVENT(EVT_TRACK_SELECTION_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_TRACK_DOWNLOAD, wxCommandEvent);

class TrackPanel : public wxPanel {
  public:
    explicit TrackPanel(wxWindow *parent, Downloader *_downloader);

    Downloader *get_downloader() const;

    void Refresh();
    void
    MergeTracks(const std::vector<std::shared_ptr<TrackInterface>> &_batch);
    std::vector<std::size_t> GetSelectedRows() const;
    std::vector<std::shared_ptr<TrackInterface>>
    GetTracksOfSelectedRows() const;
    void SetDownloadProgress(unsigned _row, int _progress);
    std::shared_ptr<TrackInterface> GetTrack(std::size_t _row) const;
    void ApplyChangeToSelectedRows(LocalTrack::tag_type_t _type,
                                   const std::string &_value);
    void Search(const wxString &_query, bool _showVerified,
                bool _showUnverified);

  private:
    wxDataViewCtrl *ctrl{};
    TrackModel *model{};
    Downloader *downloader;

    void OnSelectionChanged(wxDataViewEvent &);
    void OnActivated(wxDataViewEvent &event);
    void OnColumnSorted(wxDataViewEvent &_event);
    void OnLeftDown(wxMouseEvent &event);
};