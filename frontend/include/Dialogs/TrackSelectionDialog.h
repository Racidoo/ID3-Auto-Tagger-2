#pragma once

#include <memory>
#include <wx/wx.h>

#include "Services/TagService.h"

class ITrack;
class TrackPanel;

class TrackSelectionDialog : public wxDialog {
  public:
    TrackSelectionDialog(wxWindow *_parent,
                         const TagService::TagResearchResult &_result);
    ~TrackSelectionDialog() = default;

    std::shared_ptr<ITrack> getSelectedTrack();

  private:
    TrackPanel *trackPanel;
};
