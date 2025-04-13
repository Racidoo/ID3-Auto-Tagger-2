#include "../include/PlaylistLabel.h"
#include "../include/TrackLabel.h"

PlaylistLabel::PlaylistLabel(wxWindow *_parent,
                             const Spotify::Playlist &_playlist)
    : MediaLabel(_parent,
                 loadImageFromURL(_playlist.get_imageUrl(), wxSize(150, 150)),
                 _playlist.get_name(), {_playlist.get_creator().get_name()}),
      spotifyPlaylist(std::make_unique<Spotify::Playlist>(_playlist)) {}