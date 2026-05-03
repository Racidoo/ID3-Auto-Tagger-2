#include "../include/TrackEditWindow.h"
#include "../include/LabeledTextCtrl.h"
#include "../include/MediaLabel.h"
#include "../include/TrackLabel.h"

TrackEditWindow::TrackEditWindow(wxWindow *_parent, wxWindowID _winid,
                                 const wxPoint &_pos, const wxSize &_size)
    : wxPanel(_parent, _winid, _pos, _size) {

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    albumCover = new wxStaticBitmap(this, wxID_ANY, wxBitmap(300, 300));

    auto attributesSizer(new wxBoxSizer(wxVERTICAL));

    titleText = new LabeledTextCtrl(this, wxID_ANY,
                                    LocalTrack::tag_type_t::TITLE, "Trackname");
    artistText = new LabeledTextCtrl(
        this, wxID_ANY, LocalTrack::tag_type_t::ARTIST, "Trackartist");
    albumText = new LabeledTextCtrl(this, wxID_ANY,
                                    LocalTrack::tag_type_t::ALBUM, "Album");
    albumArtistsText = new LabeledTextCtrl(
        this, wxID_ANY, LocalTrack::tag_type_t::ALBUM_ARTIST, "Albumartist");
    genreText = new LabeledTextCtrl(this, wxID_ANY,
                                    LocalTrack::tag_type_t::GENRE, "Genre");
    labelText = new LabeledTextCtrl(
        this, wxID_ANY, LocalTrack::tag_type_t::LABEL, "Label/Producer");
    yearText = new LabeledTextCtrl(this, wxID_ANY, LocalTrack::tag_type_t::YEAR,
                                   "Year");
    trackNumberText = new LabeledTextCtrl(
        this, wxID_ANY, LocalTrack::tag_type_t::TRACK, "Track-Nr.");
    discNumberText = new LabeledTextCtrl(
        this, wxID_ANY, LocalTrack::tag_type_t::DISC, "Disc-Nr.");
    copyrightText = new LabeledTextCtrl(
        this, wxID_ANY, LocalTrack::tag_type_t::COPYRIGHT, "Copyright");
    filenameText = new LabeledTextCtrl(
        this, wxID_ANY, LocalTrack::tag_type_t::FILENAME, "Filename");
    auto fileExtensionText = new LabeledTextCtrl(
        this, wxID_ANY, LocalTrack::tag_type_t::FILENAME, "");

    fileExtensionText->Disable();
    fileExtensionText->SetValue(".mp3");

    attributesSizer->Add(titleText, 1, wxEXPAND, 5);
    auto artistSizer = new wxBoxSizer(wxHORIZONTAL);
    auto albumSizer = new wxBoxSizer(wxHORIZONTAL);
    auto albumArtistSizer = new wxBoxSizer(wxHORIZONTAL);
    auto filenameSizer = new wxBoxSizer(wxHORIZONTAL);
    artistSizer->Add(artistText, 1, wxEXPAND, 5);
    artistSizer->Add(yearText, 0, wxTILE, 5);
    albumSizer->Add(albumText, 1, wxEXPAND, 5);
    albumSizer->Add(trackNumberText, 0, wxTILE, 5);
    albumArtistSizer->Add(albumArtistsText, 1, wxEXPAND, 5);
    albumArtistSizer->Add(discNumberText, 0, wxTILE, 5);
    filenameSizer->Add(filenameText, 1, wxEXPAND, 5);
    filenameSizer->Add(fileExtensionText, 0, wxSHRINK, 5);
    attributesSizer->Add(artistSizer, 1, wxEXPAND, 5);
    attributesSizer->Add(albumSizer, 1, wxEXPAND, 5);
    attributesSizer->Add(albumArtistSizer, 1, wxEXPAND, 5);
    attributesSizer->Add(genreText, 1, wxEXPAND, 5);
    attributesSizer->Add(labelText, 1, wxEXPAND, 5);
    attributesSizer->Add(copyrightText, 1, wxEXPAND, 5);
    attributesSizer->Add(filenameSizer, 1, wxEXPAND, 5);

    mainSizer->Add(albumCover, 1, wxSHRINK, 5);
    mainSizer->Add(attributesSizer, 1, wxEXPAND, 5);
    this->SetSizerAndFit(mainSizer);

    Bind(wxEVT_SHOW, &TrackEditWindow::OnShow, this);
}

TrackEditWindow::~TrackEditWindow() {}

void TrackEditWindow::set_selected(
    const std::vector<std::shared_ptr<TrackInterface>> &_selected) {
    selected = _selected;
}

void TrackEditWindow::Update() {

    albumCover->SetBitmap(getCommonBitmap(
        albumCover->GetSize(), [](auto t) { return t->get_cover(); }));
    titleText->SetValue(
        getCommonAttribute([](auto t) { return t->get_title(); }));
    artistText->SetValue(
        getCommonAttribute([](auto t) { return t->get_artist(); }));
    albumText->SetValue(
        getCommonAttribute([](auto t) { return t->get_album(); }));
    yearText->SetValue(
        getCommonAttribute([](auto t) { return t->get_year(); }));
    trackNumberText->SetValue(
        getCommonAttribute([](auto t) { return t->get_trackNumber(); }));
    genreText->SetValue(
        getCommonAttribute([](auto t) { return t->get_genre(); }));
    albumArtistsText->SetValue(
        getCommonAttribute([](auto t) { return t->get_albumArtist(); }));
    discNumberText->SetValue(
        getCommonAttribute([](auto t) { return t->get_discNumber(); }));
    labelText->SetValue(
        getCommonAttribute([](auto t) { return t->get_label(); }));
    copyrightText->SetValue(
        getCommonAttribute([](auto t) { return t->get_copyright(); }));
    filenameText->SetValue(
        getCommonAttribute([](auto t) { return t->get_id(); }));

    this->GetSizer()->Layout();
}

void TrackEditWindow::OnShow(wxShowEvent &_event) {
    if (selected.empty()) {
        // we don't want to show an empty edit window
        Hide();
    }
    Update();
    _event.Skip();
}

std::size_t TrackEditWindow::bitmapHash(const wxBitmap &bmp) {
    if (!bmp.IsOk())
        return 0;

    wxImage img = bmp.ConvertToImage();
    if (!img.IsOk())
        return 0;

    std::string buffer;

    // Append RGB data
    const unsigned char *rgb = img.GetData();
    if (rgb) {
        buffer.append(reinterpret_cast<const char *>(rgb),
                      img.GetWidth() * img.GetHeight() * 3);
    }

    // Append Alpha if present
    if (img.HasAlpha()) {
        const unsigned char *alpha = img.GetAlpha();
        buffer.append(reinterpret_cast<const char *>(alpha),
                      img.GetWidth() * img.GetHeight());
    }

    return std::hash<std::string>{}(buffer);
}

bool TrackEditWindow::bitmapsEqual(const wxBitmap &a, const wxBitmap &b) {
    return (bitmapHash(a) == bitmapHash(b));
}
