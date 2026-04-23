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

    SetScrollRate(60, 60);

    content = new wxPanel(this);
    contentSizer = new wxBoxSizer(wxVERTICAL);
    content->SetSizer(contentSizer);
    content->SetAutoLayout(true);

    auto *outerSizer = new wxBoxSizer(wxVERTICAL);
    outerSizer->Add(content, 1, wxEXPAND);
    SetSizer(outerSizer);

    Bind(EVT_TRACK_DELETE, [this](wxCommandEvent &_event) {
        auto id = _event.GetString().ToStdString();
        auto label = getLabel(id);
        if (!label) {
            wxLogError(wxT("failed to locate " + id));
            return;
        }
        downloader->deleteLocalTrack(label->get_data());
        deleteChild(label);
    });

    Bind(EVT_TRACK_VERIFY, [this](wxCommandEvent &_event) {
        std::string id = _event.GetString().utf8_string();
        auto label = getLabel(id);
        if (!label) {
            auto it = std::find_if(
                trackLabels.begin(), trackLabels.end(), [id](const auto &_obj) {
                    return (_obj.second->get_data()
                                ? _obj.second->get_data()->get_id() == id
                                : false);
                });
            if (it == trackLabels.end()) {
                std::cerr << __FILE__ << ":" << __LINE__
                          << " Could not locate '" << id << "'" << std::endl;
                return;
            }
            std::cout << "Detected renaming of TrackLabel '" << id << "'"
                      << std::endl;
            label = it->second;
            auto node = trackLabels.extract(it);
            node.key() = id;
            auto it_newLabel = trackLabels.insert(std::move(node));
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

void TrackWindow::appendChild(std::shared_ptr<TrackInterface> data) {
    auto *trackLabel = new TrackLabel(content, data);

    contentSizer->Add(trackLabel, 0, wxALL | wxEXPAND, 5);
    trackLabels[data->get_id()] = trackLabel;
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