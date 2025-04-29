#if !defined(ICON_PROVIDER_H)
#define ICON_PROVIDER_H

#include <wx/artprov.h>

#define wxART_BRAND_SPOTIFY wxART_MAKE_ART_ID(wxID_HIGHEST +1)
#define wxART_BRAND_YOUTUBE wxART_MAKE_ART_ID(wxID_HIGHEST +2)
#define wxART_CIRCLE_USER wxART_MAKE_ART_ID(wxID_HIGHEST +3)
#define wxART_CIRCLE_CHECKMARK wxART_MAKE_ART_ID(wxID_HIGHEST+ 4)
#define wxART_CIRCLE_PLAY wxART_MAKE_ART_ID(wxID_HIGHEST +5)
#define wxART_CIRCLE_XMARK wxART_MAKE_ART_ID(wxID_HIGHEST+ 6)
#define wxART_CIRCLE_DOWNLOAD wxART_MAKE_ART_ID(wxID_HIGHEST +7)
#define wxART_CUSTOMIZE wxART_MAKE_ART_ID(wxID_HIGHEST+ 8)
#define wxART_DOWNLOADED wxART_MAKE_ART_ID(wxID_HIGHEST +9)
#define wxART_SEARCH wxART_MAKE_ART_ID(wxID_HIGHEST + 10)
#define wxART_SEARCH_ALL wxART_MAKE_ART_ID(wxID_HIGHEST + 11)
#define wxART_TRACK wxART_MAKE_ART_ID(wxID_HIGHEST + 12)
#define wxART_ALBUM wxART_MAKE_ART_ID(wxID_HIGHEST + 13)
#define wxART_ARTIST wxART_MAKE_ART_ID(wxID_HIGHEST + 14)
#define wxART_PLAYLIST wxART_MAKE_ART_ID(wxID_HIGHEST + 15)

#define wxART_ARROW_RIGHT wxART_MAKE_ART_ID(wxID_HIGHEST + 16)

#define wxART_ALBUMCOVER_PREVIEW "albumCoverPreview"
#define wxART_ALBUMCOVER_FULL "albumCoverFull"

class IconProvider : public wxArtProvider {
  protected:
    wxBitmap CreateBitmap(const wxArtID &_id, const wxArtClient &_client,
                          const wxSize &_size) override;
};

#endif // ICON_PROVIDER_H
