#if !defined(VIDEO_LABEL_H)
#define VIDEO_LABEL_H

#include <memory>
#include <wx/wx.h>

#include "MediaLabel.h"
#include "YouTube/Video.h"

// wxDECLARE_EVENT(EVT_VIDEO_DOWNLOAD, wxCommandEvent);

class VideoLabel : public MediaLabel {
  private:
    std::unique_ptr<YouTube::Video> youTubeVideo;

    void onDownloadButtonClick(wxMouseEvent &event);

  public:
    VideoLabel(wxWindow *_parent, std::unique_ptr<YouTube::Video> _video);

    inline const YouTube::Video *get_youTubeVideo() const {
        return youTubeVideo.get();
    }
    inline const QueryObject *getObject() const override {
        return youTubeVideo.get();
    }
};
#endif // VIDEO_LABEL_H
