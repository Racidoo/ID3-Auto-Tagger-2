#include "AlbumDetailsPanel.h"
#include "Downloader.h"
#include "Interfaces/IAlbum.h"
#include "MediaLabel.h"
#include "TrackPanel.h"

AlbumDetailsPanel::AlbumDetailsPanel(wxWindow *_parent,
                                     std::shared_ptr<IMediaEntity> _source,
                                     Downloader *_downloader)
    : MediaDetailsPanel(_parent, _source) {

    if (!source) {
        wxLogError(wxT("invalid reference to source"));
    }
    auto album = std::dynamic_pointer_cast<IAlbum>(source);
    if (!album) {
        wxLogDebug(wxT("album"));
    }

    auto albumCoverBitmap = new wxStaticBitmap(
        this, wxID_ANY,
        MediaLabel::loadImage(album->get_cover(), wxSize(300, 300)));
    auto albumTypeText =
        new wxStaticText(this, wxID_ANY, wxString::FromUTF8(album->get_type()));
    auto titleText =
        new wxStaticText(this, wxID_ANY, wxString::FromUTF8(album->get_title()),
                         wxDefaultPosition, wxSize(-1, 150));

    auto artistCoverBitmap = new wxStaticBitmap(
        this, wxID_ANY,
        MediaLabel::loadImage(album->get_artistCover(), wxSize(32, 32)));
    auto artistText = new wxStaticText(this, wxID_ANY,
                                       wxString::FromUTF8(album->get_artist()));
    auto yearText =
        new wxStaticText(this, wxID_ANY, wxString::FromUTF8(album->get_year()));

    auto headerSizer = new wxBoxSizer(wxHORIZONTAL);
    auto infoSizer = new wxBoxSizer(wxVERTICAL);
    auto artistSizer = new wxBoxSizer(wxHORIZONTAL);
    auto contentSizer = new wxBoxSizer(wxVERTICAL);

    artistSizer->Add(artistCoverBitmap, 0, wxALIGN_LEFT | wxALL, 5);
    artistSizer->Add(artistText, 0, wxALIGN_LEFT | wxALL, 5);
    artistSizer->Add(yearText, 0, wxALIGN_LEFT | wxALL, 5);
    infoSizer->Add(albumTypeText, 0, wxALIGN_LEFT | wxALL, 5);
    infoSizer->Add(titleText, 0, wxALIGN_LEFT | wxALL, 5);
    infoSizer->Add(artistSizer, 0, wxALIGN_LEFT | wxALL, 5);
    headerSizer->Add(albumCoverBitmap, 0, wxALIGN_LEFT | wxALL, 5);
    headerSizer->Add(infoSizer, 0, wxALIGN_LEFT | wxALL, 5);

    auto trackPanel = new TrackPanel(this, nullptr);
    contentSizer->Add(trackPanel, 1, wxALL, 5);
    if (!album->isMetaDataComplete()) {
        _downloader->get_spotify()->loadAdditionalData(album);
    }
    trackPanel->MergeTracks(album->get_tracklist());

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(headerSizer, 0, wxALIGN_TOP | wxALL, 5);
    mainSizer->Add(contentSizer, 1, wxALIGN_TOP | wxALL, 5);

    SetSizer(mainSizer);
}
