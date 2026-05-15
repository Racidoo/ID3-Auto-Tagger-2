#pragma once

#include <wx/wx.h>

#include "LocalTrack.h" // tag_type_t
#include "Media/Track/TrackPanel.h"

class LocalTrackPanel : public TrackPanel {
  public:
    explicit LocalTrackPanel(wxWindow *_parent, Downloader *_downloader);
    ~LocalTrackPanel() = default;

    Downloader *get_downloader() const;

    void ApplyChangeToSelectedRows(LocalTrack::tag_type_t _type,
                                   const std::string &_value);

  protected:
    void OnActivated(wxDataViewEvent &event);

    Downloader *downloader;
};
