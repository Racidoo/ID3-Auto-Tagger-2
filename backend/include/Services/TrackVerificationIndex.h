#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

class TrackVerificationIndex {
  public:
    TrackVerificationIndex();
    ~TrackVerificationIndex();

    TrackVerificationIndex(const TrackVerificationIndex &) = delete;
    TrackVerificationIndex &operator=(const TrackVerificationIndex &) = delete;
    TrackVerificationIndex(TrackVerificationIndex &&) = delete;
    TrackVerificationIndex &operator=(TrackVerificationIndex &&) = delete;

    bool isVerifiedTrack(const std::string &_spotifyId) const;

    void addToIndex(const std::string &_spotifyId, const std::string &_artist,
                    const std::string &_title);
    void removeFromIndex(const std::string &_spotifyId);

    bool loadOrCreateIndex();
    bool writeIndex();

  private:
    const inline static std::string relativeFilepath = "index.json";
    json index;
    bool dirty;
};