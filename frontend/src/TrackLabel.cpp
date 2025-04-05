#include "../include/TrackLabel.h"
#include "../include/ScrollText.h"

wxDEFINE_EVENT(EVT_TRACK_DOWNLOAD, wxCommandEvent);

TrackLabel::TrackLabel(wxWindow *_parent, const wxString &_songPath)
    : wxPanel(_parent, wxID_ANY, wxDefaultPosition) {

    TagLib::MPEG::File track(_songPath);

    auto *checkmark = new wxStaticBitmap(
        this, wxID_ANY, wxBitmap(wxT("img/check-circle.png"), wxBITMAP_TYPE_PNG));

    create(loadImageFromTag(track), track.tag()->title().toCString(),
           track.tag()->artist().toCString(), track.tag()->album().toCString(),
           track.tag()->genre().toCString(), {checkmark});
}

TrackLabel::TrackLabel(wxWindow *_parent, const Spotify::Track &_track)
    : wxPanel(_parent, wxID_ANY, wxDefaultPosition),
      spotifyTrack(std::make_unique<Spotify::Track>(_track)) {

    wxBitmapButton *downloadButton = new wxBitmapButton(
        this, wxID_ANY,
        wxBitmap(wxT("img/progress-download.png"), wxBITMAP_TYPE_PNG),
        wxDefaultPosition, wxSize(64, 64), wxBORDER_NONE);
    downloadButton->Bind(wxEVT_BUTTON, &TrackLabel::onDownloadButtonClick,
                         this);

    create(loadImageFromURL(spotifyTrack->get_album().get_imageUrl()),
           spotifyTrack->get_name(), spotifyTrack->get_stringArtists(),
           spotifyTrack->get_album().get_name(), "Unknown genre",
           {downloadButton});
}

TrackLabel::~TrackLabel() {}

void TrackLabel::create(const wxBitmap &_albumCover, const wxString &_title,
                        const wxString &_artist, const wxString &_album,
                        const wxString &_genre,
                        std::vector<wxObject *> elements) {

    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

    int columnWidths[] = {250, 250, 150, 150}; // Title, Artist, Album, Genre

    // Cover Image (Fixed size)
    auto *coverBitmap = new wxStaticBitmap(this, wxID_ANY, _albumCover,
                                           wxDefaultPosition, wxSize(64, 64));

    // **Title + Artist in a Vertical BoxSizer**
    auto *titleText = new ScrollText(this, wxID_ANY, _title, wxDefaultPosition,
                                     wxSize(columnWidths[0], -1));
    auto *artistText =
        new ScrollText(this, wxID_ANY, _artist, wxDefaultPosition,
                       wxSize(columnWidths[1], -1));
    artistText->SetForegroundColour(wxColour(150, 150, 150));

    auto *albumText = new ScrollText(this, wxID_ANY, _album, wxDefaultPosition,
                                     wxSize(columnWidths[2], -1));
    auto *genreText = new ScrollText(this, wxID_ANY, _genre, wxDefaultPosition,
                                     wxSize(columnWidths[3], -1));

    auto *textSizer = new wxBoxSizer(wxVERTICAL);
    textSizer->Add(titleText, 0, wxALIGN_CENTER, 5);
    textSizer->Add(artistText, 0, wxALIGN_CENTER, 5);
    auto *infoSizer = new wxBoxSizer(wxHORIZONTAL);
    infoSizer->Add(coverBitmap, 0, wxALIGN_CENTER, 5);
    infoSizer->Add(textSizer, 1, wxALIGN_LEFT, 10);
    infoSizer->Add(albumText, 0, wxALIGN_CENTER, 5);
    infoSizer->Add(genreText, 0, wxALIGN_CENTER, 5);

    auto *elementsSizer = new wxBoxSizer(wxHORIZONTAL);

    for (wxObject *obj : elements) {
        if (wxWindow *window = dynamic_cast<wxWindow *>(obj)) {
            elementsSizer->Add(window, 0, wxCENTER, 5);
        } else if (wxSizer *nestedSizer = dynamic_cast<wxSizer *>(obj)) {
            elementsSizer->Add(nestedSizer, 1, wxEXPAND);
        }
    }

    infoSizer->Add(elementsSizer, 1, wxCENTER);

    // Add the row layout to the main sizer
    sizer->Add(infoSizer, 1);

    this->SetSizer(sizer);
    this->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    coverBitmap->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    albumText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    genreText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
}

void TrackLabel::onClick(wxMouseEvent &event) {
    m_isActive = !m_isActive;
    if (m_isActive) {
        // m_activeSongs.push_back(m_songPath);
        this->SetBackgroundColour(*wxLIGHT_GREY);
    } else {
        // auto it =
        //     std::find(m_activeSongs.begin(), m_activeSongs.end(),
        //     m_songPath);
        // if (it != m_activeSongs.end()) {
        //     m_activeSongs.erase(it);
        // }
        this->SetBackgroundColour(wxNullColour);
    }
    this->Refresh();
}
void TrackLabel::onDownloadButtonClick(wxCommandEvent &event) {
    wxCommandEvent trackEvent(EVT_TRACK_DOWNLOAD);
    if (!spotifyTrack) {
        wxLogError("spotifyTrack invalid pointer");
    }
    if (spotifyTrack->get_id().empty()) {
        wxLogError("ID empty");
    }
    std::cout << "track id '" << spotifyTrack->get_id().c_str() << "'"
              << std::endl;
    std::cout.flush();
    trackEvent.SetString(spotifyTrack->get_id().c_str());
    wxPostEvent(GetParent(), trackEvent); // Send event to parent

    // wxCommandEvent testEvent(EVT_TRACK_DOWNLOAD_COMPLETE);
    // wxPostEvent(this, testEvent);
}

wxBitmap TrackLabel::loadImageFromURL(const wxString &url) {
    wxImage image;
    if (wxFileExists(url)) {

        // Load from file
        image.LoadFile(url, wxBITMAP_TYPE_ANY);
        if (!image.IsOk()) {
            wxLogError("Failed to load image: %s", url);
            return wxNullBitmap;
        }
        return wxBitmap(image);
    }
    wxHTTP http;
    http.SetHeader("User-Agent", "Mozilla/5.0"); // Mimic a browser
    http.SetTimeout(10);                         // 10 seconds timeout

    // Parse the URL
    wxURI uri(url);
    wxString host = uri.GetServer();
    wxString path = uri.GetPath();
    if (path.IsEmpty())
        path = "/";

    // Connect to the host
    if (!http.Connect(host)) {
        wxLogError("Failed to connect to host: %s", host);
        return wxBitmap();
    }

    // Get the image as a stream
    wxInputStream *stream = http.GetInputStream(path);
    if (!stream || !stream->IsOk()) {
        wxLogError("Failed to get input stream from %s", url);
        return wxBitmap();
    }
    wxMemoryOutputStream memStream;
    stream->Read(memStream);
    wxMemoryInputStream inputStream(memStream);
    if (image.LoadFile(inputStream, wxBITMAP_TYPE_ANY))
        return wxBitmap(image.Rescale(64, 64));

    wxLogError("Failed to load image from stream");

    return wxBitmap(50, 50); // Return empty bitmap if load fails
}

wxBitmap TrackLabel::loadImageFromTag(TagLib::MPEG::File &track) {
    // Get ID3v2 tag
    auto tag = track.ID3v2Tag();
    if (!tag) {
        // wxLogError("No ID3v2 tag found in: %s", track.name());
        return wxBitmap();
    }

    // Search for an APIC (Attached Picture) frame
    TagLib::ID3v2::FrameList frames = tag->frameListMap()["APIC"];
    if (frames.isEmpty()) {
        // wxLogError("No album cover found in: %s", track.name());
        return wxBitmap();
    }

    // Extract image data
    auto *apic =
        dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());
    if (!apic) {
        // wxLogError("Invalid APIC frame in: %s", track.name());
        return wxBitmap();
    }

    // Get raw image data
    TagLib::ByteVector imageData = apic->picture();
    if (imageData.isEmpty()) {
        // wxLogError("APIC frame contains no image data in: %s", track.name());
        return wxBitmap();
    }

    // Convert to wxWidgets image
    wxMemoryInputStream imgStream(imageData.data(), imageData.size());
    wxImage image;
    if (!image.LoadFile(imgStream, wxBITMAP_TYPE_ANY)) {
        // wxLogError("Failed to load album cover from: %s", track.name());
        return wxBitmap();
    }

    return wxBitmap(image.Rescale(64, 64));
}

// void TrackLabel::onTaskComplete(wxCommandEvent &) {
//     this->SetBackgroundColour(*wxGREEN); // Example: Change background color
//     this->Refresh();                     // Refresh UI
//     wxLogMessage("TrackLabel received event from parent!");
// }