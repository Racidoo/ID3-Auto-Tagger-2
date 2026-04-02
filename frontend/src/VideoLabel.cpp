#include "../include/VideoLabel.h"

// wxDEFINE_EVENT(EVT_VIDEO_DOWNLOAD, wxCommandEvent);

VideoLabel::VideoLabel(wxWindow *_parent,
                       std::unique_ptr<YouTube::Video> _video)
    : MediaLabel(_parent,
                 loadImageFromURL(_video->get_imageUrl(), wxSize(150, 150)),
                 _video->get_name(),
                 {wxString(_video->get_uploadDate() + " - " +
                           _video->get_channelTitle())}),
      youTubeVideo(std::move(_video)) {}

// void VideoLabel::onDownloadButtonClick(wxMouseEvent &event) {

//     if (!youTubeVideo || youTubeVideo->get_id().empty()) {
//         wxLogDebug(wxT("no data | full progress"));
//         return;
//     }
//     wxCommandEvent trackEvent(EVT_VIDEO_DOWNLOAD);
//     trackEvent.SetString(youTubeVideo->get_id().c_str());
//     wxPostEvent(GetParent(), trackEvent); // Send event to parent
//     wxLogDebug("sent Event");
// }