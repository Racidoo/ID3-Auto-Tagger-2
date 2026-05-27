#pragma once

#include <wx/wx.h>

#include "Services/TagService.h"
#include "Media/Track/TrackPanel.h"

class ComparisonTrackPanel : public TrackPanel {
  public:
    explicit ComparisonTrackPanel(wxWindow *_parent);
    ~ComparisonTrackPanel() = default;

    // void ApplyChangeToSelectedRows(LocalTrack::tag_type_t _type,
    //                                const std::string &_value);

  // protected:
    // virtual bool
    // HandleColumnAction(int _column, std::size_t _rowIndex,
    //                    const std::shared_ptr<TrackModelRow> &_row,
    //                    const std::shared_ptr<ITrack> &_track) override;
};
