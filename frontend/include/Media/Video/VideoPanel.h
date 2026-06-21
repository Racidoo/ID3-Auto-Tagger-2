#pragma once

#include <memory>
#include <unordered_map>
#include <wx/dataview.h>
#include <wx/wx.h>

wxDECLARE_EVENT(EVT_VIDEO_DOWNLOAD, wxCommandEvent);
wxDECLARE_EVENT(EVT_VIDEO_SELECTION_CHANGED, wxCommandEvent);

class IVideo;
class VideoModel;
class VideoModelRow;

class VideoPanel : public wxPanel {
  public:
    explicit VideoPanel(wxWindow *_parent);

    void Refresh();
    void Merge(const std::vector<std::shared_ptr<IVideo>> &_batch);
    std::vector<std::size_t> GetSelectedRows() const;
    std::vector<std::shared_ptr<IVideo>> GetVideosOfSelectedRows() const;
    void SetDownloadProgress(std::size_t _row, int _progress);
    void UpdateRow(std::size_t _row);
    std::shared_ptr<IVideo> GetVideo(std::size_t _row) const;

    void Search(const wxString &_query);

  protected:
    wxDataViewCtrl *ctrl{};
    VideoModel *model{};

    void OnSelectionChanged(wxDataViewEvent &);
    void OnActivated(wxDataViewEvent &event);
    void OnColumnSorted(wxDataViewEvent &_event);
    void OnLeftDown(wxMouseEvent &event);
    virtual bool HandleColumnAction(int _column, std::size_t _rowIndex,
                                    const std::shared_ptr<VideoModelRow> &_row,
                                    const std::shared_ptr<IVideo> &_track);
};