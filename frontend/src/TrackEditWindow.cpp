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
    attributesSizer->Add(copyrightText, 1, wxEXPAND, 5);

    mainSizer->Add(albumCover, 1, wxSHRINK, 5);
    mainSizer->Add(attributesSizer, 1, wxEXPAND, 5);
    this->SetSizerAndFit(mainSizer);

    this->Bind(EVT_TRACKLABEL_CLICKED, [this](wxCommandEvent &event) {
        TrackLabel *clickedLabel =
            dynamic_cast<TrackLabel *>(event.GetEventObject());
        if (!clickedLabel) {
            wxLogMessage("Could not derive TrackLabel from %s",
                         event.GetEventObject());
            return;
        }
        if (activeSongs.contains(clickedLabel)) {
             wxLogMessage("erase");
            clickedLabel->SetBackgroundColour(wxNullColour);
            activeSongs.erase(clickedLabel);
        } else {
             wxLogMessage("insert");
            clickedLabel->SetBackgroundColour(*wxLIGHT_GREY);
            activeSongs.insert(clickedLabel);
        }
        // if (GetParent()) {
        //     wxCommandEvent notifyEvent(EVT_SHOW_TRACK_DETAILS, GetId());
        //     notifyEvent.SetEventObject(this);
        //     wxPostEvent(GetParent(), notifyEvent);
        // }

        if (get_activeSongs().empty()) {
             wxLogMessage("hide");
            this->Hide();
            this->GetParent()->Layout();
            return;
        }
             wxLogMessage("show");
        this->Show();
        this->GetParent()->Layout();
        this->show();
    });

    this->Bind(EVT_VALUE_CHANGE, [this](wxCommandEvent &event) {
        std::string value = event.GetString().ToStdString();
        LocalTrack::tag_type_t type =
            static_cast<LocalTrack::tag_type_t>(event.GetInt());
        std::cout << "Caught event: " << value << " type: " << type
                  << std::endl;
        for (auto activeSong : get_activeSongs()) {
            auto track = activeSong->get_data();
            if (!track)
                return;
            switch (type) {
            case LocalTrack::tag_type_t::TITLE: {
                track->set_title(value);
                break;
            }
            case LocalTrack::tag_type_t::ARTIST: {
                track->set_artist(value);
                break;
            }
            case LocalTrack::tag_type_t::ALBUM: {
                track->set_album(value);
                break;
            }
            case LocalTrack::tag_type_t::ALBUM_ARTIST: {
                track->set_albumArtist(value);
                break;
            }
            case LocalTrack::tag_type_t::COPYRIGHT: {
                track->set_copyright(value);
                break;
            }
            case LocalTrack::tag_type_t::GENRE: {
                track->set_genre(value);
                break;
            }
            case LocalTrack::tag_type_t::LABEL: {
                track->set_label(value);
                break;
            }
            case LocalTrack::tag_type_t::YEAR: {
                track->set_year(value);
                break;
            }
            case LocalTrack::tag_type_t::TRACK: {
                track->set_track(value);
                break;
            }
            case LocalTrack::tag_type_t::DISC: {
                track->set_disc(value);
                break;
            }
            }
            activeSong->Update();
        }
    });
}

TrackEditWindow::~TrackEditWindow() {}

void TrackEditWindow::show() {

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
        getCommonAttribute([](auto t) { return t->get_track(); }));
    genreText->SetValue(
        getCommonAttribute([](auto t) { return t->get_genre(); }));
    albumArtistsText->SetValue(
        getCommonAttribute([](auto t) { return t->get_albumArtist(); }));
    discNumberText->SetValue(
        getCommonAttribute([](auto t) { return t->get_disc(); }));
    labelText->SetValue(
        getCommonAttribute([](auto t) { return t->get_label(); }));
    copyrightText->SetValue(
        getCommonAttribute([](auto t) { return t->get_copyright(); }));

    this->GetSizer()->Layout();
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
