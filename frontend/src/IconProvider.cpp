#include "../include/IconProvider.h"

wxBitmap IconProvider::CreateBitmap(const wxArtID &_id,
                                    const wxArtClient &_client,
                                    const wxSize &_size) {

    wxSize size = _size.IsFullySpecified()
                      ? _size
                      : (_client == wxART_TOOLBAR              ? wxSize(32, 32)
                         : _client == wxART_MENU               ? wxSize(16, 16)
                         : _client == wxART_BUTTON             ? wxSize(32, 32)
                         : _client == wxART_ALBUMCOVER_PREVIEW ? wxSize(64, 64)
                         : _client == wxART_ALBUMCOVER_FULL ? wxSize(150, 150)
                                                            : wxSize(32, 32));

    if (_id == wxART_BRAND_SPOTIFY) {
        wxImage img(wxT("img/spotify_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_BRAND_YOUTUBE) {
        wxImage img(wxT("img/youtube_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_CIRCLE_CHECKMARK) {
        wxImage img(wxT("img/check-circle_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_CIRCLE_PLAY) {
        wxImage img(wxT("img/play-circle_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_CIRCLE_XMARK) {
        wxImage img(wxT("img/circle-xmark_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_CIRCLE_DOWNLOAD) {
        wxImage img(wxT("img/progress-download_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_CUSTOMIZE) {
        wxImage img(wxT("img/customize_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_DOWNLOADED) {
        wxImage img(wxT("img/down-to-line_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_SEARCH) {
        wxImage img(wxT("img/music-magnifying-glass_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_SEARCH_ALL) {
        wxImage img(wxT("img/symbols_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_TRACK) {
        wxImage img(wxT("img/music-note_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_ALBUM) {
        wxImage img(wxT("img/record-vinyl_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_ARTIST) {
        wxImage img(wxT("img/circle-user_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }
    if (_id == wxART_PLAYLIST) {
        wxImage img(wxT("img/list-music_256.png"), wxBITMAP_TYPE_PNG);
        return wxBitmap(img.Rescale(size.GetWidth(), size.GetHeight(),
                                    wxIMAGE_QUALITY_HIGH));
    }

    return wxNullBitmap;
}