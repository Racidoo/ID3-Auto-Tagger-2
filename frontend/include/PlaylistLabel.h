#if !defined(PLAYLIST_LABEL_H)
#define PLAYLIST_LABEL_H

#include <memory>
#include <wx/wx.h>

#include "Interfaces/IPlaylist.h"
#include "MediaLabel.h"

class PlaylistLabel : public MediaLabel {

  public:
    PlaylistLabel(wxWindow *_parent, std::shared_ptr<IPlaylist> _playlist);

    std::shared_ptr<IPlaylist> get_source() const;

  private:
    std::shared_ptr<IPlaylist> source;
};

#endif // PLAYLIST_LABEL_H
