#if !defined(DOWNLOADER_H)
#define DOWNLOADER_H

#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "Spotify/SpotifyAPI.h"
#include "YouTube.h"

class Downloader {
  private:
    Spotify::SpotifyAPI spotify;
    YouTube youTube;

    json blacklist;

    bool readBlacklist();
    bool writeBlacklist() const;

    bool isBlocked(const std::string &_id) const;
    void makeBlocked(const Spotify::Track &_track);

    bool downloadAndTag(const Spotify::Track &_track);

  public:
    Downloader(/* args */);
    ~Downloader();

    void verifyTags();
    // std::string determineOrigin(const std::string &_url) const;
    std::string fetchResource(const std::string &_url,
                              std::vector<Spotify::Track> &_tracks);
    std::string downloadResource(const std::vector<Spotify::Track> &_tracks);
};

#endif // DOWNLOADER_H
