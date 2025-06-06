#include "../include/TrackWindow.h"
#include "../include/Downloader.h"
#include "../include/LabeledTextCtrl.h"
#include "../include/TrackLabel.h"
#include <wx/event.h>

TrackWindow::TrackWindow(wxWindow *_parent, Downloader* _downloader)
    : wxScrolledWindow(_parent, wxID_ANY), downloader(_downloader) {
    this->SetScrollRate(15, 15);
    this->SetSizer(new wxBoxSizer(wxVERTICAL));
    Bind(EVT_TRACKLABEL_CLICKED, &TrackWindow::processClickedLabel, this);
    Bind(EVT_TRACK_DELETE, [this] (wxCommandEvent&_event){
        downloader->deleteLocalTrack(_event.GetString().ToStdString());
    });
    Bind(EVT_TRACK_VERIFY, [this] (wxCommandEvent&_event){
        downloader->get_spotify()->verifyTags(_event.GetString().ToStdString());
    });
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
    headerSizer->Add(titleHeader, 0, wxALL, 5);
    headerSizer->Add(albumHeader, 0, wxALL, 5);
    headerSizer->Add(genreHeader, 0, wxALL, 5);
    headerSizer->Add(lengthHeader, 0, wxALL, 5);
    GetSizer()->Add(headerSizer);
    GetSizer()->AddSpacer(10);
}

void TrackWindow::sortByHeader() {}

/**
 * @brief Clears elements of Sizer and clears trackLabels.
 * Clears all children of the window and trackLabels map.
 */
void TrackWindow::deleteChildren() {
    // Clear the sizer (all child elements)
    GetSizer()->Clear(true);
    trackLabels.clear();
    activeSongs.clear();
    createHeader();
}

/**
 * @brief Appends a new TrackLabel to the sizer and the trackLabels map.
 * @param _trackLabel The new TrackLabel to be added.
 */
void TrackWindow::appendChildren(TrackLabel *_trackLabel) {
    GetSizer()->Add(_trackLabel, 0, wxEXPAND, 5);
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


