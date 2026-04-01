#include "../include/TrackWindow.h"
#include "../../backend/include/LocalTrackService.h"
#include "../include/Downloader.h"
#include "../include/LabeledTextCtrl.h"
#include "../include/TrackLabel.h"
#include <wx/event.h>

wxDEFINE_EVENT(EVT_TRACKWINDOW_SCROLL_BOTTOM, wxCommandEvent);
wxDEFINE_EVENT(EVT_SHOW_TRACK_DETAILS, wxCommandEvent);

TrackWindow::TrackWindow(wxWindow *_parent, Downloader *_downloader)
    : wxScrolledWindow(_parent, wxID_ANY), downloader(_downloader) {

    SetScrollRate(15, 15);

    content = new wxPanel(this);
    contentSizer = new wxBoxSizer(wxVERTICAL);
    content->SetSizer(contentSizer);
    content->SetAutoLayout(true);

    auto *outerSizer = new wxBoxSizer(wxVERTICAL);
    outerSizer->Add(content, 1, wxEXPAND);
    SetSizer(outerSizer);

    // Bind(EVT_TRACKLABEL_CLICKED, &TrackWindow::processClickedLabel, this);
    Bind(EVT_TRACK_DELETE, [this](wxCommandEvent &_event) {
        auto id = _event.GetString().ToStdString();
        auto label = getLabel(id);
        if (!label) {
            return;
        }
        downloader->deleteLocalTrack(label->get_data()->local->get_filepath());
        deleteChild(label);
    });
    Bind(EVT_TRACK_VERIFY, [this](wxCommandEvent &_event) {
        std::string id = _event.GetString().utf8_string();
        auto label = getLabel(id);
        if (!label) {
            std::cerr << __FILE__ << ":" << __LINE__ << " Could not locate '"
                      << id << "'" << std::endl;
            return;
        }
        TrackInterface::verify(label->get_data(), downloader);

        label->Update();
        Layout();
    });

    Bind(wxEVT_SCROLLWIN_THUMBTRACK, [this](wxScrollWinEvent &evt) {
        evt.Skip();
        int pos = GetScrollPos(wxVERTICAL);
        int range = GetScrollRange(wxVERTICAL);
        int page = GetScrollPageSize(wxVERTICAL);

        if (pos >= range - page) {
            wxCommandEvent notify(
                EVT_TRACKWINDOW_SCROLL_BOTTOM); // or a custom event
            wxPostEvent(GetParent(), notify);
        }
    });
}

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

void TrackWindow::appendChild(
    std::shared_ptr<TrackInterface::TrackViewData> data) {
    // Construct label with correct parent
    auto *trackLabel = new TrackLabel(content, data);

    // Defer addition until content panel is realized to avoid GTK crash
    wxTheApp->CallAfter([this, trackLabel, data]() {
        if (!contentSizer)
            return;

        contentSizer->Add(trackLabel, 0, wxALL | wxEXPAND, 5);

        trackLabels[data->get_id()] = trackLabel;

        contentSizer->Layout();
        FitInside();
    });
}

void TrackWindow::filterTracks(const std::string &query) {
    if (!contentSizer)
        return;

    // Only update visibility if needed
    for (auto &[id, label] : trackLabels) {
        const bool visible = query.empty() || LocalTrackService::matchesSearch(
                                                  label->get_data(), query);

        // Only update if visibility changed
        if (label->IsShown() != visible) {
            label->Show(visible);
        }
    }

    // Re-layout once after all visibility updates
    contentSizer->Layout();
    FitInside();
}

void TrackWindow::Clear() {
    if (!contentSizer)
        return;

    for (auto &[id, label] : trackLabels) {
        label->Destroy();
    }
    trackLabels.clear();

    contentSizer->Clear(true); // also deletes sizer items
    Layout();
    FitInside();
}

void TrackWindow::deleteChild(TrackLabel *_trackLabel) {
    if (!_trackLabel)
        return;

    // Remove from sizer
    wxSizer *sizer = GetContentSizer();
    if (sizer) {
        sizer->Detach(_trackLabel);
    }

    // Remove from trackLabels map
    trackLabels.erase(_trackLabel->get_data()->get_id());

    // Destroy the widget (safe in wxWidgets)
    _trackLabel->Destroy();

    // Layout update
    Layout();
}

/**
 * @brief Scroll event handler to track scrolling actions.
 * This will be triggered when the user interacts with the scrollbar.
 */
void TrackWindow::OnScroll(wxScrollWinEvent &event) {
    // You can handle the scroll event here (e.g., log or perform specific
    // actions). For example, log the new scroll position.
    wxLogMessage("Scroll position: %d", event.GetPosition());
    event.Skip(); // Allow other handlers to process the event
}

TrackLabel *TrackWindow::getLabel(const std::string &_id) {
    auto it = trackLabels.find(_id);
    if (it != trackLabels.end()) {
        return it->second;
    }
    return nullptr;
}