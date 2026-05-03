#include "../include/TrackLabel.h"
#include "../include/CircleProgressBar.h"
#include "../include/IconProvider.h"
#include "../include/MediaLabel.h"
#include "../include/ScrollText.h"
#include "../include/Spotify/Track.h"

wxDEFINE_EVENT(EVT_TRACK_DOWNLOAD, wxCommandEvent);
wxDEFINE_EVENT(EVT_TRACK_VERIFY, wxCommandEvent);
wxDEFINE_EVENT(EVT_TRACK_DELETE, wxCommandEvent);
wxDEFINE_EVENT(EVT_TRACKLABEL_CLICKED, wxCommandEvent);

TrackLabel::TrackLabel(wxWindow *_parent, std::shared_ptr<TrackInterface> _data)
    : wxPanel(_parent, wxID_ANY, wxDefaultPosition), isActive(false),
      data(_data) {
    Create();
    Update();
}

TrackLabel::~TrackLabel() {}

std::shared_ptr<TrackInterface> TrackLabel::get_data() const { return data; }

void TrackLabel::Create() {
    rootSizer = new wxFlexGridSizer(1, 8, 0, 0);
    auto *textSizer = new wxBoxSizer(wxVERTICAL);

    progressBar = new CircleProgressBar(this);
    coverBitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap());
    titleText = new ScrollText(this, wxID_ANY, "title");
    artistText = new ScrollText(this, wxID_ANY, "artist");
    artistText->SetForegroundColour(wxColour(150, 150, 150));
    albumText = new ScrollText(this, wxID_ANY, "album");
    genreText = new ScrollText(this, wxID_ANY, "genre");
    lengthText = new wxStaticText(this, wxID_ANY, "length");
    lengthText->Enable(false);

    textSizer->Add(titleText, 0, wxEXPAND);
    textSizer->Add(artistText, 0, wxEXPAND);

    actionVerify = new wxBitmapButton(
        this, wxID_ANY,
        wxBitmap(wxArtProvider::GetBitmap(wxART_ASSESSMENT, wxART_BUTTON)));
    actionDelete = new wxBitmapButton(
        this, wxID_ANY,
        wxBitmap(wxArtProvider::GetBitmap(wxART_TRASH_XMARK, wxART_BUTTON)));

    actionVerify->Show();
    actionDelete->Show();

    progressBar->SetMinSize(wxSize(columnWidths[0], columnWidths[0]));
    // progressBar->SetMaxSize(wxSize(columnWidths[0], -1));
    coverBitmap->SetMinSize(wxSize(columnWidths[1], columnWidths[1]));
    titleText->SetMinSize(wxSize(columnWidths[2], -1));
    artistText->SetMinSize(wxSize(columnWidths[2], -1));
    albumText->SetMinSize(wxSize(columnWidths[3], -1));
    genreText->SetMinSize(wxSize(columnWidths[4], -1));
    lengthText->SetMinSize(wxSize(columnWidths[5], -1));
    actionVerify->SetMinSize(wxSize(columnWidths[6], -1));
    actionDelete->SetMinSize(wxSize(columnWidths[7], -1));

    rootSizer->Add(progressBar, 1, wxEXPAND, 5);
    rootSizer->Add(coverBitmap, 0, wxALL, 5);
    rootSizer->Add(textSizer, 0, wxEXPAND | wxALL, 5);
    rootSizer->Add(albumText, 0, wxEXPAND | wxALL, 5);
    rootSizer->Add(genreText, 0, wxEXPAND | wxALL, 5);
    rootSizer->Add(lengthText, 0, wxEXPAND | wxALL, 5);
    rootSizer->Add(actionVerify, 0, wxEXPAND | wxALL, 5);
    rootSizer->Add(actionDelete, 0, wxEXPAND | wxALL, 5);

    rootSizer->AddGrowableCol(2); // Title
    rootSizer->AddGrowableCol(3); // Album
    rootSizer->AddGrowableCol(4); // Genre

    if (!data->get_localTrack()) {
        actionVerify->Hide();
        actionDelete->Hide();
    }

    this->SetSizerAndFit(rootSizer);

    progressBar->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onDownloadButtonClick,
                      this);
    // Bind clicks once
    this->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    titleText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    artistText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    coverBitmap->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    albumText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    genreText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
}

void TrackLabel::Update() {
    coverBitmap->SetBitmap(MediaLabel::loadImage(
        data->get_cover(), wxSize(columnWidths[1], columnWidths[1])));
    titleText->SetLabel(wxString::FromUTF8(data->get_title()));
    artistText->SetLabel(wxString::FromUTF8(data->get_artist()));
    albumText->SetLabel(wxString::FromUTF8(data->get_album()));
    genreText->SetLabel(wxString::FromUTF8(data->get_genre()));

    int minutes = (data->get_length() / 60) % 60;
    int seconds = data->get_length() % 60;

    wxString duration =
        wxString(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") +
                 std::to_string(seconds));

    lengthText->SetLabel(duration);

    // Progress logic
    if (data->get_spotifyTrack() && data->is_verified()) {
        progressBar->SetProgress(CIRCLE_PROGRESSBAR_FINISH);
    } else if (data->get_localTrack()) {
        progressBar->SetProgress(data->is_verified()
                                     ? CIRCLE_PROGRESSBAR_FINISH
                                     : CIRCLE_PROGRESSBAR_CANCEL);

        actionVerify->Show();
        actionDelete->Show();
        actionVerify->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &_event) {
            wxCommandEvent trackEvent(EVT_TRACK_VERIFY, wxID_ANY);
            trackEvent.SetString(wxString::FromUTF8(data->get_id()));
            wxPostEvent(GetParent(), trackEvent); // Send event to parent
        });
        actionDelete->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &_event) {
            wxCommandEvent trackEvent(EVT_TRACK_DELETE);
            trackEvent.SetString(wxString::FromUTF8(data->get_id()));
            wxPostEvent(GetParent(), trackEvent); // Send event to parent
        });
    }

    Layout();
    Refresh();
}

void TrackLabel::Update(std::shared_ptr<TrackInterface> _data) {
    data = _data;
    Update();
}

/**
 * @brief
 *
 * @param event
 */
void TrackLabel::onClick(wxMouseEvent &_event) {
    if (!data->get_localTrack() || !GetParent()) {
        _event.Skip();
        return;
    }

    wxCommandEvent evt(EVT_TRACKLABEL_CLICKED, GetId());
    evt.SetEventObject(this);
    evt.SetInt(_event.ShiftDown()); // 0 = single, 1 = multi

    wxPostEvent(GetParent(), evt);

    _event.Skip();
}

/**
 * @brief
 *
 * @param event
 */
void TrackLabel::onDownloadButtonClick(wxMouseEvent &_event) {
    if (!data->get_spotifyTrack() || data->get_id().empty() ||
        data->get_localTrack() || data->is_verified() ||
        progressBar->get_progress() == CIRCLE_PROGRESSBAR_FINISH) {
        // wxLogDebug(wxT("no data | full progress"));
        return;
    }
    // wxLogDebug(wxT("spotifyTrack"));
    wxCommandEvent trackEvent(EVT_TRACK_DOWNLOAD);
    trackEvent.SetString(data->get_id());
    wxPostEvent(GetParent(), trackEvent); // Send event to parent
    _event.Skip();
}

wxSize TrackLabel::DoGetBestSize() const {
    return wxSize(500, 100); // Or however tall you want each item
}