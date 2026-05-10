#include "PlaylistLabel.h"

PlaylistLabel::PlaylistLabel(wxWindow *_parent,
                             std::shared_ptr<IPlaylist> _playlist)
    : MediaLabel(_parent, loadImage(_playlist->get_cover(), wxSize(150, 150)),
                 _playlist->get_title(),
                 {
                     // _playlist.get_creator().get_name()
                 }),
      source(_playlist) {}

std::shared_ptr<IPlaylist> PlaylistLabel::get_source() const { return source; }