#include "ArtistLabel.h"

ArtistLabel::ArtistLabel(wxWindow *_parent, std::shared_ptr<IArtist> _artist)
    : MediaLabel(_parent, loadImage(_artist->get_cover(), wxSize(150, 150)),
                 _artist->get_title(), {wxT("Artist")}),
      source(_artist) {}

std::shared_ptr<IArtist> ArtistLabel::get_source() const { return source; }