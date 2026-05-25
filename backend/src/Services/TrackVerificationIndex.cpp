#include "Services/TrackVerificationIndex.h"
#include "Spotify/SpotifyAPI.h"

TrackVerificationIndex::TrackVerificationIndex() : dirty(false) {
    loadOrCreateIndex();
}
TrackVerificationIndex::~TrackVerificationIndex() { writeIndex(); }

bool TrackVerificationIndex::isVerifiedTrack(
    const std::string &_spotifyId) const {
    return index.at("verifiedTracks").contains(_spotifyId);
}

void TrackVerificationIndex::addToIndex(const std::string &_spotifyId,
                                        const std::string &_artist,
                                        const std::string &_title) {
    if (!Spotify::SpotifyAPI::isValidIdFormat(_spotifyId) || _artist.empty() ||
        _title.empty()) {
        std::cerr << "Will not add incomplete track to index!" << std::endl;
        return;
    }
    index["verifiedTracks"][_spotifyId]["title"] = _title;
    index["verifiedTracks"][_spotifyId]["artist"] = _artist;
    dirty = true;
}

void TrackVerificationIndex::removeFromIndex(const std::string &_spotifyId) {
    if (!isVerifiedTrack(_spotifyId)) {
        return;
    }
    index["verifiedTracks"].erase(_spotifyId);
    std::cout << "removed '" << _spotifyId << "' from index" << std::endl;
    dirty = true;
}

bool TrackVerificationIndex::loadOrCreateIndex() {
    if (!std::filesystem::exists(relativeFilepath)) {

        index = {{"verifiedTracks", json::object()}};

        std::ofstream file(relativeFilepath);
        file << index.dump(4);
        return false;
    }

    std::ifstream file(relativeFilepath);
    if (!file.is_open()) {
        std::cerr << "Could not open " << relativeFilepath << "!" << std::endl;
        return false;
    }

    file >> index;
    return true;
}

bool TrackVerificationIndex::writeIndex() {
    if (!dirty) {
        return false;
    }
    static const std::string tempPath = relativeFilepath + ".tmp";
    {
        std::ofstream blacklistFile(tempPath);
        if (!blacklistFile.is_open()) {
            std::cerr << "Could not write " << tempPath << "!" << std::endl;
            return false;
        }
        blacklistFile << index.dump();
    }
    std::filesystem::rename(tempPath, relativeFilepath);
    dirty = false;
    return true;
}
