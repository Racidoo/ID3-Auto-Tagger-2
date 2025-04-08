#include "../include/TrackWindow.h"
// #include <wx/scrolwin.h>
#include <wx/event.h>

TrackWindow::TrackWindow(wxWindow *_parent)
    : wxScrolledWindow(_parent, wxID_ANY),
      mainSizer(new wxBoxSizer(wxVERTICAL)) {

    this->SetScrollRate(15, 15);
    this->SetSizer(mainSizer);
    Bind(EVT_TRACKLABEL_CLICKED, &TrackWindow::processClickedLabel, this);
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
    // _trackLabel->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &evt) {
    //     wxWindow *clicked = dynamic_cast<wxWindow *>(evt.GetEventObject());
    //     wxLogMessage("Clicked child: %p", clicked);
    //     evt.Skip();
    // });
    if (_trackLabel->get_spotifyTrack()) {
        trackLabels.insert(
            {_trackLabel->get_spotifyTrack()->get_id(), _trackLabel});
        return;
    }
    if (_trackLabel->get_localTrack()) {
        trackLabels.insert(
            {_trackLabel->get_localTrack()->name(), _trackLabel});
        return;
    }
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

void TrackWindow::processClickedLabel(wxCommandEvent &event) {
    // wxLogDebug(wxT("processClickedLabel()"));
    TrackLabel *clickedLabel =
        dynamic_cast<TrackLabel *>(event.GetEventObject());
    if (!clickedLabel) {
        wxLogMessage("Could not derive TrackLabel from %s",
                     event.GetEventObject());
        return;
    }
    if (activeSongs.contains(clickedLabel)) {
        clickedLabel->SetBackgroundColour(wxNullColour);
        activeSongs.erase(clickedLabel);
    } else {
        clickedLabel->SetBackgroundColour(*wxLIGHT_GREY);
        activeSongs.insert(clickedLabel);
    }
    if (GetParent()) {
        wxCommandEvent notifyEvent(EVT_SHOW_TRACK_DETAILS, GetId());
        notifyEvent.SetEventObject(this);
        wxPostEvent(GetParent(), notifyEvent);
    }
}
