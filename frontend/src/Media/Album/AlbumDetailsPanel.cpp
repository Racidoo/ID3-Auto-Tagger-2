#include "Media/Album/AlbumDetailsPanel.h"
#include "Components/MediaLabel.h"
#include "Downloader.h"
#include "Interfaces/IAlbum.h"
#include "Media/Track/TrackPanel.h"

AlbumDetailsPanel::AlbumDetailsPanel(wxWindow *_parent,
                                     std::shared_ptr<IMediaEntity> _source,
                                     Downloader *_downloader)
    : MediaDetailsPanel(_parent, _source) {

    if (!source) {
        wxLogError(wxT("invalid reference to source"));
        return;
    }
    auto album = std::dynamic_pointer_cast<IAlbum>(source);
    if (!album) {
        wxLogError(wxT("invalid album"));
        return;
    }

    if (!album->isMetaDataComplete()) {
        _downloader->get_spotify()->loadAdditionalData(album);
    }

    std::size_t totalLength(0);
    for (auto &&track : album->get_tracklist()) {
        totalLength += track->get_length();
    }

    int hours = (totalLength / 60) / 60;
    int minutes = (totalLength / 60) % 60;
    int seconds = totalLength % 60;

    wxString duration = (hours > 0 ? std::to_string(hours) + ":" : "") +
                        (std::to_string(minutes) + ":" +
                         (seconds < 10 ? "0" : "") + std::to_string(seconds));

    auto albumCoverBitmap = new wxStaticBitmap(
        this, wxID_ANY,
        MediaLabel::loadImage(album->get_cover(), wxSize(256, 265)));

    auto albumTypeText =
        new wxStaticText(this, wxID_ANY, wxString::FromUTF8(album->get_type()));
    auto typeFont = albumTypeText->GetFont();
    typeFont.SetPointSize(8);
    typeFont.SetWeight(wxFONTWEIGHT_LIGHT);
    albumTypeText->SetFont(typeFont);

    auto titleText = new wxStaticText(this, wxID_ANY,
                                      wxString::FromUTF8(album->get_title()));
    auto titleFont = titleText->GetFont();
    // ToDo: Adjust later, based of the length and available space from parent
    // window
    titleFont.SetPointSize(32);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleText->SetFont(titleFont);

    auto artistCoverBitmap = new wxStaticBitmap(
        this, wxID_ANY,
        MediaLabel::loadImage(album->get_artistCover(), wxSize(32, 32)));
    auto artistText = new wxStaticText(this, wxID_ANY,
                                       wxString::FromUTF8(album->get_artist()));
    auto yearText =
        new wxStaticText(this, wxID_ANY, wxString::FromUTF8(album->get_year()));
    auto totalTracksText = new wxStaticText(
        this, wxID_ANY,
        wxString(std::to_string(album->get_tracklist().size()) + " tracks"));
    auto totalDurationText = new wxStaticText(this, wxID_ANY, duration);

    auto metaFont = artistText->GetFont();
    metaFont.SetPointSize(8);

    artistText->SetFont(metaFont);
    yearText->SetFont(metaFont);
    totalTracksText->SetFont(metaFont);
    totalDurationText->SetFont(metaFont);

    auto headerSizer = new wxBoxSizer(wxHORIZONTAL);
    auto infoSizer = new wxBoxSizer(wxVERTICAL);
    auto artistSizer = new wxBoxSizer(wxHORIZONTAL);
    auto contentSizer = new wxBoxSizer(wxVERTICAL);

    artistSizer->Add(artistCoverBitmap, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL,
                     5);
    artistSizer->Add(artistText, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
    artistSizer->Add(yearText, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
    artistSizer->Add(totalTracksText, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
    artistSizer->Add(totalDurationText, 0, wxALIGN_CENTER_VERTICAL);

    infoSizer->Add(albumTypeText, 0, wxBOTTOM, 5);
    infoSizer->Add(titleText, 0, wxBOTTOM, 15);
    infoSizer->AddStretchSpacer();
    infoSizer->Add(artistSizer, 0, wxBOTTOM, 5);

    headerSizer->Add(albumCoverBitmap, 0, wxALL, 10);
    headerSizer->Add(infoSizer, 1, wxEXPAND | wxTOP | wxBOTTOM | wxRIGHT, 10);

    auto trackPanel = new TrackPanel(this, nullptr);
    contentSizer->Add(trackPanel, 1, wxEXPAND | wxALL, 5);
    trackPanel->MergeTracks(album->get_tracklist());

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(headerSizer, 0, wxALIGN_TOP | wxALL, 5);
    mainSizer->Add(contentSizer, 1, wxEXPAND | wxALL, 5);

    SetSizer(mainSizer);
}
