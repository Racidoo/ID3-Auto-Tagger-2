#pragma once

#include "Discogs/DiscogsAPI.h"
#include "Local/LocalTrackService.h"
#include "Spotify/SpotifyAPI.h"
#include "YouTube/YouTubeAPI.h"

class MediaServiceRegistry {
  public:
    Discogs::DiscogsAPI discogs;
    LocalTrackService local;
    Spotify::SpotifyAPI spotify;
    YouTube::YouTubeAPI youtube;

    IMediaService &get_spotify() { return spotify; }
    IMediaService &get_discogs() { return discogs; }
    IMediaService &get_youtube() { return youtube; }
    IMediaService &get_local() { return local; }
};