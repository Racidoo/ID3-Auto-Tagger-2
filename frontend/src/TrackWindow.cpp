#include "../include/TrackWindow.h"
#include "../include/LabeledTextCtrl.h"
#include <wx/event.h>

TrackWindow::TrackWindow(wxWindow *_parent)
    : wxScrolledWindow(_parent, wxID_ANY),
      mainSizer(new wxBoxSizer(wxVERTICAL)) {

    this->SetScrollRate(15, 15);
    this->SetSizer(mainSizer);
    Bind(EVT_TRACKLABEL_CLICKED, &TrackWindow::processClickedLabel, this);
}

TrackWindow::~TrackWindow() {}

void TrackWindow::createHeader() {
    auto headerSizer = new wxFlexGridSizer(1, 6, 5, 5);
    auto titleHeader =
        new wxStaticText(this, wxID_ANY, wxT("Title"), wxDefaultPosition,
                         wxSize(TrackLabel::columnWidths[2], -1));
    auto albumHeader =
        new wxStaticText(this, wxID_ANY, wxT("Album"), wxDefaultPosition,
                         wxSize(TrackLabel::columnWidths[3], -1));
    auto genreHeader =
        new wxStaticText(this, wxID_ANY, wxT("Genre"), wxDefaultPosition,
                         wxSize(TrackLabel::columnWidths[4], -1));
    auto lengthHeader =
        new wxStaticText(this, wxID_ANY, wxT("Length"), wxDefaultPosition,
                         wxSize(TrackLabel::columnWidths[5], -1));

    headerSizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString,
                                      wxDefaultPosition,
                                      wxSize(TrackLabel::columnWidths[0], -1)),
                     0, wxEXPAND, 5);
    headerSizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString,
                                      wxDefaultPosition,
                                      wxSize(TrackLabel::columnWidths[1], -1)),
                     0, wxEXPAND, 5);
    headerSizer->Add(titleHeader, 0, wxEXPAND, 5);
    headerSizer->Add(albumHeader, 0, wxEXPAND, 5);
    headerSizer->Add(genreHeader, 0, wxEXPAND, 5);
    headerSizer->Add(lengthHeader, 0, wxEXPAND, 5);
    GetSizer()->Add(headerSizer);
    GetSizer()->AddSpacer(10);
}

void TrackWindow::sortByHeader(){
     
}


/**
 * @brief Clears elements of Sizer and clears trackLabels.
 * Clears all children of the window and trackLabels map.
 */
void TrackWindow::deleteChildren() {
    // Clear the sizer (all child elements)
    this->GetSizer()->Clear(true);
    trackLabels.clear();
    activeSongs.clear();
    createHeader();
}

/**
 * @brief Appends a new TrackLabel to the sizer and the trackLabels map.
 * @param _trackLabel The new TrackLabel to be added.
 */
void TrackWindow::appendChildren(TrackLabel *_trackLabel) {
    mainSizer->Add(_trackLabel, 0, wxEXPAND, 5);
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
