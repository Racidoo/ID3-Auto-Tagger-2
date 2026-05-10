#if !defined(VIDEO_LABEL_H)
#define VIDEO_LABEL_H

#include <memory>
#include <wx/wx.h>

#include "MediaLabel.h"

class IVideo;

class VideoLabel : public MediaLabel {

  public:
    VideoLabel(wxWindow *_parent, std::shared_ptr<IVideo> _video);

    std::shared_ptr<IVideo> get_source() const;

  private:
    std::shared_ptr<IVideo> source;

    void onDownloadButtonClick(wxMouseEvent &event);
};
#endif // VIDEO_LABEL_H
