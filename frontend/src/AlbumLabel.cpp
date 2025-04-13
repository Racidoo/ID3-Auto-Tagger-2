#include "../include/AlbumLabel.h"
#include "../include/TrackLabel.h"

AlbumLabel::AlbumLabel(wxWindow *_parent, const Spotify::Album &_album)
    : MediaLabel(_parent,
                 loadImageFromURL(_album.get_imageUrl(), wxSize(150, 150)),
                 _album.get_name(),
                 {wxString(_album.get_releaseDate().substr(0, 4) + " - " +
                           _album.get_artists())}),
      spotifyAlbum(std::make_unique<Spotify::Album>(_album)) {}