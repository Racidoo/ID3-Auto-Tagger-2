#if !defined(MEDIA_LABEL_H)
#define MEDIA_LABEL_H

#include <taglib/attachedpictureframe.h>
#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>
#include <wx/mstream.h>
#include <wx/url.h>
#include <wx/wx.h>

class MediaLabel : public wxPanel {
  public:
    MediaLabel(wxWindow *_parent, const wxBitmap &_cover,
               const wxString &_title, const std::vector<wxString> &_infoLines);

    static wxBitmap loadImageFromURL(const wxString &_url, const wxSize &_size);
    static wxBitmap loadImageFromTag(TagLib::MPEG::File *_track,
                                     const wxSize &_size);
};

#endif // MEDIA_LABEL_H
