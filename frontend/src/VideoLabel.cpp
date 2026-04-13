#include "../include/VideoLabel.h"

VideoLabel::VideoLabel(wxWindow *_parent,
                       std::unique_ptr<YouTube::Video> _video)
    : MediaLabel(_parent,
                 loadImage(_video->get_image(), wxSize(150, 150)),
                 _video->get_name(),
                 {wxString(_video->get_uploadDate() + " - " +
                           _video->get_channelTitle())}),
      youTubeVideo(std::move(_video)) {}
