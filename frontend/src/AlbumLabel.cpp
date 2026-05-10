#include "AlbumLabel.h"

AlbumLabel::AlbumLabel(wxWindow *_parent, std::shared_ptr<IAlbum> _album)
    : MediaLabel(_parent, loadImage(_album->get_cover(), wxSize(150, 150)),
                 _album->get_title(), {}
                 //  {wxString(_album.get_releaseDate().substr(0, 4) + " - " +
                 //            _album.get_artists())}
                 ),
      source(_album) {}

std::shared_ptr<IAlbum> AlbumLabel::get_source() const { return source; }