#include "../include/TrackEditWindow.h"
#include "../include/TrackLabel.h"

TrackEditWindow::TrackEditWindow(wxWindow *_parent, wxWindowID _winid,
                                 const wxPoint &_pos, const wxSize &_size)
    : wxPanel(_parent, _winid, _pos, _size) {

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    albumCover = new wxStaticBitmap(this, wxID_ANY, wxBitmap(300, 300));

    auto attributesSizer(new wxBoxSizer(wxVERTICAL));

    titleText = new LabeledTextCtrl(this, wxID_ANY, "Trackname");
    artistText = new LabeledTextCtrl(this, wxID_ANY, "Trackartist");
    albumText = new LabeledTextCtrl(this, wxID_ANY, "Album");
    albumArtistsText = new LabeledTextCtrl(this, wxID_ANY, "Albumartist");
    genreText = new LabeledTextCtrl(this, wxID_ANY, "Genre");
    labelText = new LabeledTextCtrl(this, wxID_ANY, "Label/Producer");
    yearText = new LabeledTextCtrl(this, wxID_ANY, "Year");
    trackNumberText = new LabeledTextCtrl(this, wxID_ANY, "Track-Nr.");
    discNumberText = new LabeledTextCtrl(this, wxID_ANY, "Disc-Nr.");

    attributesSizer->Add(titleText, 1, wxEXPAND, 5);
    auto artistSizer = new wxBoxSizer(wxHORIZONTAL);
    auto albumSizer = new wxBoxSizer(wxHORIZONTAL);
    auto albumArtistSizer = new wxBoxSizer(wxHORIZONTAL);
    artistSizer->Add(artistText, 1, wxEXPAND, 5);
    artistSizer->Add(yearText, 0, wxTILE, 5);
    albumSizer->Add(albumText, 1, wxEXPAND, 5);
    albumSizer->Add(trackNumberText, 0, wxTILE, 5);
    albumArtistSizer->Add(albumArtistsText, 1, wxEXPAND, 5);
    albumArtistSizer->Add(discNumberText, 0, wxTILE, 5);
    attributesSizer->Add(artistSizer, 1, wxEXPAND, 5);
    attributesSizer->Add(albumSizer, 1, wxEXPAND, 5);
    attributesSizer->Add(albumArtistSizer, 1, wxEXPAND, 5);
    attributesSizer->Add(genreText, 1, wxEXPAND, 5);
    attributesSizer->Add(labelText, 1, wxEXPAND, 5);

    mainSizer->Add(albumCover, 1, wxSHRINK, 5);
    mainSizer->Add(attributesSizer, 1, wxEXPAND, 5);
    this->SetSizerAndFit(mainSizer);

    this->Bind(EVT_VALUE_CHANGE, [this](wxCommandEvent &event) {
        wxLogDebug(event.GetString());
    });
}

TrackEditWindow::~TrackEditWindow() {}

void TrackEditWindow::show(const std::vector<TagLib::MPEG::File *> &_tracks) {

    titleText->SetValue(getCommonAttribute(
        _tracks, [](auto *tag) { return tag->title().toCString(); }));
    artistText->SetValue(getCommonAttribute(
        _tracks, [](auto *tag) { return tag->artist().toCString(); }));
    albumText->SetValue(getCommonAttribute(
        _tracks, [](auto *tag) { return tag->album().toCString(); }));
    albumArtistsText->SetValue(getCommonAttribute(_tracks, [](auto *tag) {
        return tag->frameListMap()["TPE2"][0]->toString().toCString();
    }));
    genreText->SetValue(getCommonAttribute(
        _tracks, [](auto *tag) { return tag->genre().toCString(); }));
    labelText->SetValue(getCommonAttribute(_tracks, [](auto *tag) {
        return tag->frameListMap()["TPUB"][0]->toString().toCString();
    }));
    yearText->SetValue(getCommonAttribute(
        _tracks, [](auto *tag) { return std::to_string(tag->year()); }));
    trackNumberText->SetValue(getCommonAttribute(
        _tracks, [](auto *tag) { return std::to_string(tag->track()); }));
    discNumberText->SetValue(getCommonAttribute(_tracks, [](auto *tag) {
        return tag->frameListMap()["TPOS"][0]->toString().toCString();
    }));

    this->GetSizer()->Layout();
}