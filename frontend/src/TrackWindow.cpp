#include "../include/TrackWindow.h"
#include "../../backend/include/LocalTrackService.h"
#include "../include/Downloader.h"
#include "../include/LabeledTextCtrl.h"
#include "../include/TrackLabel.h"
#include <wx/event.h>

wxDEFINE_EVENT(EVT_TRACKWINDOW_SCROLL_BOTTOM, wxCommandEvent);
wxDEFINE_EVENT(EVT_SHOW_TRACK_DETAILS, wxCommandEvent);
wxDEFINE_EVENT(EVT_SORT_CHANGED, wxCommandEvent);

TrackWindow::TrackWindow(wxWindow *_parent, Downloader *_downloader)
    : wxPanel(_parent, wxID_ANY), downloader(_downloader) {

    auto *mainSizer = new wxBoxSizer(wxVERTICAL);

    headerPanel = new wxPanel(this);
    contentPanel = new wxScrolledWindow(this);

    contentPanel->SetScrollRate(60, 60);

    contentSizer = new wxBoxSizer(wxVERTICAL);
    contentPanel->SetSizer(contentSizer);

    mainSizer->Add(headerPanel, 0, wxEXPAND);  // fixed height
    mainSizer->Add(contentPanel, 1, wxEXPAND); // fills remaining space

    SetSizer(mainSizer);

    createHeader();

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

    contentPanel->Bind(
        wxEVT_SCROLLWIN_THUMBTRACK, [this](wxScrollWinEvent &evt) {
            evt.Skip();
            int pos = contentPanel->GetScrollPos(wxVERTICAL);
            int range = contentPanel->GetScrollRange(wxVERTICAL);
            int page = contentPanel->GetScrollPageSize(wxVERTICAL);

            if (pos >= range - page) {
                wxCommandEvent notify(
                    EVT_TRACKWINDOW_SCROLL_BOTTOM); // or a custom event
                wxPostEvent(GetParent(), notify);
            }
        });
}

void TrackWindow::createHeader() {
    auto headerSizer = new wxFlexGridSizer(1, 8, 0, 0);

    auto headerProgress = new wxPanel(headerPanel);
    auto headerCoverBitmap =
        new wxStaticText(headerPanel, wxID_ANY, wxEmptyString);
    auto headerTitle = new wxStaticText(headerPanel, wxID_ANY, wxT("Title"));
    auto headerAlbum = new wxStaticText(headerPanel, wxID_ANY, wxT("Album"));
    auto headerGenre = new wxStaticText(headerPanel, wxID_ANY, wxT("Genre"));
    auto headerLength = new wxStaticText(headerPanel, wxID_ANY, wxT("Length"));
    auto headerActionDelete =
        new wxStaticText(headerPanel, wxID_ANY, wxEmptyString);
    auto headerActionVerify =
        new wxStaticText(headerPanel, wxID_ANY, wxEmptyString);

    headerTitle->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &) {
        sendSortEvent(SortKey::Title);
    });
    headerAlbum->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &) {
        sendSortEvent(SortKey::Album);
    });
    headerGenre->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &) {
        sendSortEvent(SortKey::Genre);
    });
    headerLength->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &) {
        sendSortEvent(SortKey::Length);
    });

    headerProgress->SetMinSize(wxSize(TrackLabel::columnWidths[0], -1));
    headerProgress->SetMaxSize(wxSize(TrackLabel::columnWidths[0], -1));
    headerCoverBitmap->SetMinSize(wxSize(TrackLabel::columnWidths[1], -1));
    headerTitle->SetMinSize(wxSize(TrackLabel::columnWidths[2], -1));
    headerAlbum->SetMinSize(wxSize(TrackLabel::columnWidths[3], -1));
    headerGenre->SetMinSize(wxSize(TrackLabel::columnWidths[4], -1));
    headerLength->SetMinSize(wxSize(TrackLabel::columnWidths[5], -1));
    headerActionDelete->SetMinSize(wxSize(TrackLabel::columnWidths[6], -1));
    headerActionVerify->SetMinSize(wxSize(TrackLabel::columnWidths[7], -1));

    headerSizer->Add(headerProgress, 0, wxALIGN_CENTER_VERTICAL, 5);
    headerSizer->Add(headerCoverBitmap, 0, wxALIGN_CENTER_VERTICAL, 5);
    headerSizer->Add(headerTitle, 1, wxEXPAND, 5);
    headerSizer->Add(headerAlbum, 1, wxEXPAND, 5);
    headerSizer->Add(headerGenre, 1, wxEXPAND, 5);
    headerSizer->Add(headerLength, 0, wxALIGN_CENTER_VERTICAL, 5);
    headerSizer->Add(headerActionDelete, 0, wxALIGN_CENTER_VERTICAL, 5);
    headerSizer->Add(headerActionVerify, 0, wxALIGN_CENTER_VERTICAL, 5);

    headerSizer->AddGrowableCol(2); // Title
    headerSizer->AddGrowableCol(3); // Album
    headerSizer->AddGrowableCol(4); // Genre

    headerPanel->SetSizer(headerSizer);
}

void TrackWindow::sendSortEvent(SortKey _key) {
    wxCommandEvent evt(EVT_SORT_CHANGED);
    evt.SetInt(static_cast<int>(_key));
    wxPostEvent(GetParent(), evt);
}

void TrackWindow::appendChild(std::shared_ptr<TrackInterface> data) {
    auto *trackLabel = new TrackLabel(contentPanel, data);

    contentSizer->Add(trackLabel, 0, wxEXPAND, 5);
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