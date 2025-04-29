#include "../include/MediaLabel.h"

wxDEFINE_EVENT(EVT_MEDIA_LABEL_CLICKED, wxCommandEvent);

MediaLabel::MediaLabel(wxWindow *_parent, const wxBitmap &_cover,
                       const wxString &_title,
                       const std::vector<wxString> &_infoLines)
    : wxPanel(_parent, wxID_ANY, wxDefaultPosition) {
    auto coverBitmap = new wxStaticBitmap(this, wxID_ANY, _cover);
    auto titleText = new wxStaticText(this, wxID_ANY, _title, wxDefaultPosition,
                                      wxSize(150, -1));
    titleText->Wrap(150);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(coverBitmap, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
    mainSizer->Add(titleText, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);

    for (auto &line : _infoLines) {
        auto infoText = new wxStaticText(this, wxID_ANY, line);
        infoText->SetForegroundColour(wxColour(150, 150, 150));
        infoText->Wrap(150);
        mainSizer->Add(infoText, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
    }

    coverBitmap->Bind(wxEVT_LEFT_DOWN, &MediaLabel::onClick, this);
    titleText->Bind(wxEVT_LEFT_DOWN, &MediaLabel::onClick, this);
    this->Bind(wxEVT_LEFT_DOWN, &MediaLabel::onClick, this);

    SetSizerAndFit(mainSizer);
}

void MediaLabel::onClick(wxMouseEvent &_event) {

    wxCommandEvent notifyEvent(EVT_MEDIA_LABEL_CLICKED, GetId());
    notifyEvent.SetEventObject(this);
    wxPostEvent(GetParent(), notifyEvent);

    _event.Skip();
}

/**
 * @brief
 *
 * @param _url
 * @param _size
 * @return wxBitmap
 */
wxBitmap MediaLabel::loadImageFromURL(const wxString &_url,
                                      const wxSize &_size) {
    if (_url.IsEmpty())
        return wxBitmap();

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

wxBitmap MediaLabel::loadImageFromTag(TagLib::MPEG::File *_track,
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