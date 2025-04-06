#include "../include/TrackWindow.h"
// #include <wx/scrolwin.h>
#include <wx/event.h>

TrackWindow::TrackWindow(wxWindow *_parent)
    : wxScrolledWindow(_parent, wxID_ANY),
      mainSizer(new wxBoxSizer(wxVERTICAL)) {

    this->SetScrollRate(15, 15); 
    this->SetSizer(mainSizer);
}

TrackWindow::~TrackWindow() {}

/**
 * @brief Clears elements of Sizer and clears trackLabels.
 * Clears all children of the window and trackLabels map.
 */
void TrackWindow::deleteChildren() {
    // Clear the sizer (all child elements)
    this->GetSizer()->Clear(true);
    trackLabels.clear();
}

/**
 * @brief Appends a new TrackLabel to the sizer and the trackLabels map.
 * @param _trackLabel The new TrackLabel to be added.
 */
void TrackWindow::appendChildren(TrackLabel *_trackLabel) {
    mainSizer->Add(_trackLabel, 0, wxEXPAND, 5);
    trackLabels.insert(
        {_trackLabel->get_spotifyTrack()->get_id(), _trackLabel});
}

/**
 * @brief Scroll event handler to track scrolling actions.
 * This will be triggered when the user interacts with the scrollbar.
 */
void TrackWindow::onScroll(wxScrollWinEvent &event) {
    // You can handle the scroll event here (e.g., log or perform specific
    // actions). For example, log the new scroll position.
    wxLogMessage("Scroll position: %d", event.GetPosition());
    event.Skip(); // Allow other handlers to process the event
}