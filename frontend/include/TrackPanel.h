#pragma once

#include <memory>
#include <wx/dataview.h>
#include <wx/wx.h>

class TrackInterface;
class TrackModel;

class TrackPanel : public wxPanel {
  public:
    explicit TrackPanel(wxWindow *parent);

    void Refresh();
    void AddTracks(
        const std::vector<std::shared_ptr<TrackInterface>> &_trackInterfaces);

  private:
    wxDataViewCtrl *ctrl{};
    TrackModel *model{};

    void OnSelectionChanged(wxDataViewEvent &event);
    void OnActivated(wxDataViewEvent &event);

    void OnColumnSorted(wxDataViewEvent &_event);
};