#pragma once

#include <memory>
#include <wx/wx.h>

class IVideo;
class VideoPanel;

class VideoSelectionDialog : public wxDialog {
  public:
    VideoSelectionDialog(wxWindow *_parent,
                         const std::vector<std::shared_ptr<IVideo>> &_result);
    ~VideoSelectionDialog() = default;

    std::shared_ptr<IVideo> getSelectedVideo();

  private:
    VideoPanel *videoPanel;
};
