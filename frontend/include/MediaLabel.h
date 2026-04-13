#if !defined(MEDIA_LABEL_H)
#define MEDIA_LABEL_H

#include <taglib/attachedpictureframe.h>
#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>
#include <wx/mstream.h>
#include <wx/wx.h>

#include "QueryObject.h"

wxDECLARE_EVENT(EVT_MEDIA_LABEL_CLICKED, wxCommandEvent);

class MediaLabel : public wxPanel {
  public:
    MediaLabel(wxWindow *_parent, const wxBitmap &_cover,
               const wxString &_title, const std::vector<wxString> &_infoLines);

    void onClick(wxMouseEvent &_event);

    virtual const QueryObject *getObject() const = 0;

    static wxBitmap loadImage(const std::vector<std::byte> &_imageData,
                              const wxSize &_size);
};

#endif // MEDIA_LABEL_H
