#if !defined(ALBUM_LABEL_H)
#define ALBUM_LABEL_H

#include <memory>
#include <wx/wx.h>

#include "Interfaces/IAlbum.h"
#include "MediaLabel.h"

class AlbumLabel : public MediaLabel {
  public:
    AlbumLabel(wxWindow *_parent, std::shared_ptr<IAlbum> _album);

    std::shared_ptr<IAlbum> get_source() const;

  private:
    std::shared_ptr<IAlbum> source;
};
#endif // ALBUM_LABEL_H
