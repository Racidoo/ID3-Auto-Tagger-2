#include "../include/MediaLabel.h"
#include "../include/ScrollText.h"

wxDEFINE_EVENT(EVT_MEDIA_LABEL_CLICKED, wxCommandEvent);

MediaLabel::MediaLabel(wxWindow *_parent, const wxBitmap &_cover,
                       const wxString &_title,
                       const std::vector<wxString> &_infoLines)
    : wxPanel(_parent, wxID_ANY, wxDefaultPosition) {
    auto coverBitmap = new wxStaticBitmap(this, wxID_ANY, _cover);
    auto titleText = new ScrollText(this, wxID_ANY, _title, wxDefaultPosition,
                                    wxSize(150, -1));
    // titleText->Wrap(150);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(coverBitmap, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
    mainSizer->Add(titleText, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);

    for (auto &line : _infoLines) {
        auto infoText = new ScrollText(this, wxID_ANY, line, wxDefaultPosition,
                                       wxSize(150, -1));
        infoText->SetForegroundColour(wxColour(150, 150, 150));
        // infoText->Wrap(150);
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

wxBitmap MediaLabel::loadImage(const std::vector<std::byte> &_imageData,
                               const wxSize &_size) {
    if (_imageData.empty()) {
        return wxBitmap(_size);
    }

    wxMemoryInputStream imgStream(_imageData.data(), _imageData.size());

    wxImage image;
    if (!image.LoadFile(imgStream, wxBITMAP_TYPE_ANY) || !image.IsOk()) {
        wxLogDebug(__FUNCTION__ + wxString(": failed to load image"));
        return wxBitmap();
    }

    return wxBitmap(image.Rescale(_size.GetWidth(), _size.GetHeight()));
}