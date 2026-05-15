#if !defined(MEDIA_LABEL_H)
#define MEDIA_LABEL_H

#include <memory>
#include <taglib/attachedpictureframe.h>
#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>
#include <wx/mstream.h>
#include <wx/wx.h>

wxDECLARE_EVENT(EVT_MEDIA_LABEL_CLICKED, wxCommandEvent);

class IMediaEntity;

class MediaLabel : public wxPanel {
  public:
    MediaLabel(wxWindow *_parent, std::shared_ptr<IMediaEntity> _source,
               const std::vector<wxString> &_infoLines);

    void onClick(wxMouseEvent &_event);

    std::shared_ptr<IMediaEntity> get_source() const;

    static wxBitmap loadImage(const std::vector<std::byte> &_imageData,
                              const wxSize &_size);

  private:
    std::shared_ptr<IMediaEntity> source;
};

#endif // MEDIA_LABEL_H
