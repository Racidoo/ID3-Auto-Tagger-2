#pragma once

#include <memory>
#include <wx/wx.h>

#include "Interfaces/ITrackMutable.h"
#include "Services/TagService.h"

class TrackPanel;

class TrackSelectionDialog : public wxDialog {
  public:
    TrackSelectionDialog(wxWindow *_parent,
                         std::shared_ptr<ITrackMutable> _current,
                         const TagService::TagResearchResult &_result);
    ~TrackSelectionDialog() = default;

    std::shared_ptr<ITrack> getSelectedTrack();

  private:
    TrackPanel *trackPanel;
};
