#pragma once

#include <wx/wx.h>

#include "Media/MediaDetailsPanel.hpp"

class Downloader;
class AlbumDetailsPanel : public MediaDetailsPanel {
  public:
    AlbumDetailsPanel(wxWindow *_parent, std::shared_ptr<IMediaEntity> _source,
                      Downloader *_downloader);
    ~AlbumDetailsPanel() = default;
};
