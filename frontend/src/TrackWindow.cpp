#include "../include/TrackWindow.h"

TrackWindow::TrackWindow(wxWindow *_parent)
    : wxScrolledWindow(_parent, wxID_ANY),
      mainSizer(new wxBoxSizer(wxVERTICAL)) {
    this->SetSizer(mainSizer);
}

TrackWindow::~TrackWindow() {}

/**
 * @brief Clears elements of Sizer nad clears trackLabels
 *
 */
void TrackWindow::deleteChildren() {
    this->GetSizer()->Clear(true);
    trackLabels.clear();
}

void TrackWindow::appendChildren(TrackLabel *_trackLabel) {
    this->GetSizer()->Add(_trackLabel, 0);
    trackLabels.insert({_trackLabel->get_spotifyTrack()->get_id(), _trackLabel});
}
