#pragma once

#include <wx/wx.h>

#include "Local/LocalTrack.h" // tag_type_t
#include "Media/Track/TrackPanel.h"

class LocalTrackPanel : public TrackPanel {
  public:
    explicit LocalTrackPanel(wxWindow *_parent);
    ~LocalTrackPanel() = default;

    void ApplyChangeToSelectedRows(LocalTrack::tag_type_t _type,
                                   const std::string &_value);

  protected:
    virtual bool
    HandleColumnAction(int _column, std::size_t _rowIndex,
                       const std::shared_ptr<TrackModelRow> &_row,
                       const std::shared_ptr<ITrack> &_track) override;
};
