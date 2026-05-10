#if !defined(ARTISTLABEL_H)
#define ARTISTLABEL_H

#include <memory>
#include <wx/wx.h>

#include "Interfaces/IArtist.h"
#include "MediaLabel.h"

class ArtistLabel : public MediaLabel {

  public:
    ArtistLabel(wxWindow *_parent, std::shared_ptr<IArtist> _artist);

    std::shared_ptr<IArtist> get_source() const;

  private:
    std::shared_ptr<IArtist> source;
};

#endif // ARTISTLABEL_H
