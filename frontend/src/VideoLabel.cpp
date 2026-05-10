#include "VideoLabel.h"
#include "Interfaces/IVideo.h"

VideoLabel::VideoLabel(wxWindow *_parent, std::shared_ptr<IVideo> _video)
    : MediaLabel(_parent, loadImage(_video->get_cover(), wxSize(150, 150)),
                 _video->get_title(),
                 {
                     // wxString(_video->get_uploadDate() + " - " +
                     //        _video->get_channelTitle())
                 }),
      source(_video) {}

std::shared_ptr<IVideo> VideoLabel::get_source() const { return source; }
