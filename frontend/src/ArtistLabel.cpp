#include "../include/ArtistLabel.h"
#include "../include/TrackLabel.h"

ArtistLabel::ArtistLabel(wxWindow *_parent, const Spotify::Artist &_artist)
    : MediaLabel(_parent,
                 loadImageFromURL(_artist.get_imageUrl(), wxSize(150, 150)),
                 _artist.get_name(), {wxT("Artist")}),
      spotifyArtist(std::make_unique<Spotify::Artist>(_artist)) {}
