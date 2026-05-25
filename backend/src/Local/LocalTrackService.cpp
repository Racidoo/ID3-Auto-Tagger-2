#include "Local/LocalTrackService.h"
#include "Local/LocalTrack.h"
#include "Services/TrackVerificationIndex.h"
#include "Spotify/SpotifyAPI.h"

LocalTrackService::LocalTrackService(const std::filesystem::path &_trackPath,
                                     TrackVerificationIndex &_index)
    : trackPath(_trackPath), verificationIndex(_index), stopRequested{false},
      generation{0} {}

LocalTrackService::~LocalTrackService() {
    if (worker.joinable())
        worker.join();
}

size_t LocalTrackService::getGeneration() { return generation; }
const std::filesystem::path &LocalTrackService::get_trackPath() const {
    return trackPath;
}
void LocalTrackService::set_trackPath(const std::filesystem::path &_path) {
    trackPath = _path;
}

void LocalTrackService::loadTracks(bool recursiveSearch) {
    stopRequested = true;

    if (worker.joinable())
        worker.join();

    stopRequested = false;

    size_t myGen = ++generation;

    worker = std::thread([this, myGen, recursiveSearch]() {
        std::vector<std::shared_ptr<ITrack>> batch;
        batch.reserve(128);

        std::error_code ec;

        for (std::filesystem::recursive_directory_iterator it(trackPath, ec),
             end;
             it != end; it.increment(ec)) {

            if (stopRequested || myGen != generation) {
                std::cerr << "stopRequested || myGen != generation"
                          << std::endl;
                return;
            }

            if (ec) {
                ec.clear();
                continue;
            }

            if (it->is_directory() && !recursiveSearch) {
                it.disable_recursion_pending();
            }

            const auto &file = *it;
            if (!file.is_regular_file() || file.path().extension() != ".mp3")
                continue;

            auto track = std::make_shared<LocalTrack>(file, this);

            track->set_verified(
                verificationIndex.isVerifiedTrack(track->get_id()));

            batch.push_back(track);

            if (batch.size() >= 32) {
                dispatchBatch(batch, myGen);
                batch.clear();
            }
        }
        dispatchBatch(batch, myGen);
    });
}

void LocalTrackService::dispatchBatch(
    std::vector<std::shared_ptr<ITrack>> &_batch, size_t _gen) {
    if (!onBatch)
        return;

    // Move batch to avoid copy
    auto movedBatch = std::move(_batch);
    onBatch(std::move(movedBatch), _gen);
}

bool LocalTrackService::renameTrack(std::shared_ptr<LocalTrack> _local,
                                    const std::string &_filename) {
    const auto &filepath = _local->get_filepath();
    std::filesystem::path newPath =
        filepath.parent_path() / (_filename + filepath.extension().string());
    try {
        std::filesystem::rename(filepath, newPath);
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Rename failed: " << e.what() << std::endl;
        return false;
    }
    _local->set_filepath(newPath);
    return true;
}

/**
 * @brief only deletes the file on disc. Deleting `this` is the user's
 * responsibility!
 *
 * @return true
 * @return false
 */
bool LocalTrackService::deleteTrack(std::shared_ptr<LocalTrack> _local) {
    const auto &filepath = _local->get_filepath();
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "Could not remove " << filepath
                  << ": File does not exist.\n";
        return false;
    }
    if (!std::filesystem::remove(filepath)) {
        std::cerr << "Could not remove " << filepath
                  << ":Failed to delete the file.\n";
        return false;
    }
    std::cout << "File deleted successfully.\n";
    return true;
}

bool LocalTrackService::load(std::shared_ptr<IMediaEntity> _obj) {
    if (auto local = std::dynamic_pointer_cast<LocalTrack>(_obj))
        return loadAdditionalData(local);
    return false;
}

bool LocalTrackService::loadAdditionalData(std::shared_ptr<LocalTrack> _local) {
    _local->ensureFullTagsLoaded();
    return false;
}
