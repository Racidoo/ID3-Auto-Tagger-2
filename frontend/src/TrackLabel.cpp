#include "../include/TrackLabel.h"
#include "../include/ScrollText.h"
#include "../include/Spotify/Track.h"

wxDEFINE_EVENT(EVT_TRACK_DOWNLOAD, wxCommandEvent);
wxDEFINE_EVENT(EVT_TRACKLABEL_CLICKED, wxCommandEvent);

TrackLabel::TrackLabel(wxWindow *_parent, const wxString &_songPath)
    : wxPanel(_parent, wxID_ANY, wxDefaultPosition) {

    localTrack = std::make_unique<TagLib::MPEG::File>(_songPath);

    TagLib::FileRef fr(TagLib::FileName(_songPath), true,
                       TagLib::AudioProperties::Accurate);
    uint length = fr.audioProperties()->length();
    create(loadImageFromTag(localTrack.get(),
                            wxSize(columnWidths[0], columnWidths[0])),
           localTrack->tag()->title().toCString(),
           localTrack->tag()->artist().toCString(),
           localTrack->tag()->album().toCString(),
           localTrack->tag()->genre().toCString(), length);
}

TrackLabel::TrackLabel(wxWindow *_parent, const Spotify::Track &_track)
    : wxPanel(_parent, wxID_ANY, wxDefaultPosition),
      spotifyTrack(std::make_unique<Spotify::Track>(_track)) {

    create(loadImageFromURL(spotifyTrack->get_album().get_imageUrl(),
                            wxSize(columnWidths[0], columnWidths[0])),
           spotifyTrack->get_name(), spotifyTrack->get_stringArtists(),
           spotifyTrack->get_album().get_name(), "Unknown genre",
           spotifyTrack->get_durationMs() / 1000);
}

TrackLabel::~TrackLabel() {}

void TrackLabel::create(const wxBitmap &_albumCover, const wxString &_title,
                        const wxString &_artist, const wxString &_album,
                        const wxString &_genre, int _length) {

    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    auto *infoSizer = new wxFlexGridSizer(1, 6, 5, 5);
    auto *textSizer = new wxBoxSizer(wxVERTICAL);

    progressBar = new CircleProgressBar(this);
    progressBar->SetMinSize(wxSize(columnWidths[0], columnWidths[0]));
    progressBar->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onDownloadButtonClick,
                      this);

    // show checkmark on local files
    if (localTrack || (spotifyTrack && spotifyTrack->isDownloaded())) {
        progressBar->SetProgress(100);
    }
    // Cover Image (Fixed size)
    auto *coverBitmap =
        new wxStaticBitmap(this, wxID_ANY, _albumCover, wxDefaultPosition,
                           wxSize(columnWidths[1], columnWidths[1]));
    // coverBitmap->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);

    // **Title + Artist in a Vertical BoxSizer**
    auto *titleText = new ScrollText(this, wxID_ANY, _title, wxDefaultPosition,
                                     wxSize(columnWidths[2], -1));
    auto *artistText =
        new ScrollText(this, wxID_ANY, _artist, wxDefaultPosition,
                       wxSize(columnWidths[2], -1));
    artistText->SetForegroundColour(wxColour(150, 150, 150));

    auto *albumText = new ScrollText(this, wxID_ANY, _album, wxDefaultPosition,
                                     wxSize(columnWidths[3], -1));
    auto *genreText = new ScrollText(this, wxID_ANY, _genre, wxDefaultPosition,
                                     wxSize(columnWidths[4], -1));

    int minutes = _length / 60;
    int seconds = _length % 60;
    // int hours = minutes / 60;
    minutes = minutes % 60;

    auto duration =
        wxString(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") +
                 std::to_string(seconds));

    auto *lengthText =
        new wxStaticText(this, wxID_ANY, duration, wxDefaultPosition,
                         wxSize(columnWidths[5], -1));
    lengthText->Enable(false);

    textSizer->Add(titleText, 0);
    textSizer->Add(artistText, 0);

    infoSizer->Add(progressBar, 1, wxEXPAND, 5);
    infoSizer->Add(coverBitmap, 0, wxSHRINK, 5);
    infoSizer->Add(textSizer, 0, wxSHRINK, 5);
    infoSizer->Add(albumText, 0, wxSHRINK, 5);
    infoSizer->Add(genreText, 0, wxSHRINK, 5);
    infoSizer->Add(lengthText, 0, wxSHRINK, 5);

    this->SetSizerAndFit(infoSizer);

    this->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    titleText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    artistText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    coverBitmap->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    albumText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
    genreText->Bind(wxEVT_LEFT_DOWN, &TrackLabel::onClick, this);
}

/**
 * @brief
 *
 * @param event
 */
void TrackLabel::onClick(wxMouseEvent &event) {
    if (localTrack && GetParent()) {
        wxCommandEvent notifyEvent(EVT_TRACKLABEL_CLICKED, GetId());
        notifyEvent.SetEventObject(this);
        wxPostEvent(GetParent(), notifyEvent);
    }
    event.Skip();
}

/**
 * @brief
 *
 * @param event
 */
void TrackLabel::onDownloadButtonClick(wxMouseEvent &event) {
    wxCommandEvent trackEvent(EVT_TRACK_DOWNLOAD);
    if (!spotifyTrack || spotifyTrack->get_id().empty() || localTrack)
        return;
    trackEvent.SetString(spotifyTrack->get_id().c_str());
    wxPostEvent(GetParent(), trackEvent); // Send event to parent
}

/**
 * @brief
 *
 * @param _url
 * @param _size
 * @return wxBitmap
 */
wxBitmap TrackLabel::loadImageFromURL(const wxString &_url,
                                      const wxSize &_size) {
    wxImage image;
    if (wxFileExists(_url)) {

        // Load from file
        image.LoadFile(_url, wxBITMAP_TYPE_ANY);
        if (!image.IsOk()) {
            wxLogError("Failed to load image: %s", _url);
            return wxNullBitmap;
        }
        return wxBitmap(image);
    }
    wxHTTP http;
    http.SetHeader("User-Agent", "Mozilla/5.0"); // Mimic a browser
    http.SetTimeout(10);                         // 10 seconds timeout

    // Parse the URL
    wxURI uri(_url);
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
        wxLogError("Failed to get input stream from %s", _url);
        return wxBitmap();
    }
    wxMemoryOutputStream memStream;
    stream->Read(memStream);
    wxMemoryInputStream inputStream(memStream);
    if (image.LoadFile(inputStream, wxBITMAP_TYPE_ANY))
        return wxBitmap(image.Rescale(_size.GetWidth(), _size.GetHeight()));

    wxLogError("Failed to load image from stream");

    return wxBitmap(50, 50); // Return empty bitmap if load fails
}

wxBitmap TrackLabel::loadImageFromTag(TagLib::MPEG::File *_track,
                                      const wxSize &_size) {
    if (!_track->hasID3v2Tag()) {
        return wxBitmap();
    }
    // Get ID3v2 tag
    auto tag = _track->ID3v2Tag();
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

    // Add missing EOI marker if needed
    if (!(imageData.size() >= 2 &&
          imageData[imageData.size() - 2] == (char)0xFF &&
          imageData[imageData.size() - 1] == (char)0xD9)) {
        imageData.append((char)0xFF);
        imageData.append((char)0xD9);
    }

    // Convert to wxWidgets image
    wxMemoryInputStream imgStream(imageData.data(), imageData.size());
    wxImage image;
    if (!image.LoadFile(imgStream, wxBITMAP_TYPE_ANY)) {
        wxLogError("Failed to load album cover from: %s", _track->name());
        return wxBitmap(64, 64);
    }

    return wxBitmap(image.Rescale(_size.GetWidth(), _size.GetHeight()));
}

wxSize TrackLabel::DoGetBestSize() const {
    return wxSize(500, 100); // Or however tall you want each item
}