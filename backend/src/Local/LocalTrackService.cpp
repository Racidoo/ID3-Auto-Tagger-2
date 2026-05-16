#include "Local/LocalTrackService.h"
#include "Downloader.h"
#include "Local/LocalTrack.h"

LocalTrackService::~LocalTrackService() {
    if (worker.joinable())
        worker.join();
}

size_t LocalTrackService::getGeneration() { return generation; }

void LocalTrackService::loadTracks(const std::filesystem::path &_path,
                                   Downloader *_downloader,
                                   bool recursiveSearch) {
    stopRequested = true;

    if (worker.joinable())
        worker.join();

    stopRequested = false;

    size_t myGen = ++generation;

    worker = std::thread([this, myGen, _downloader, _path, recursiveSearch]() {
        std::vector<std::shared_ptr<ITrack>> batch;
        batch.reserve(128);

        std::error_code ec;

        for (std::filesystem::recursive_directory_iterator it(_path, ec), end;
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

            auto track = std::make_shared<LocalTrack>(file);

            track->set_verified(_downloader->isBlocked(track->get_id()));

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

bool LocalTrackService::verify(std::shared_ptr<LocalTrack> _local,
                               Downloader *_downloader) {

    if (!_local || !_downloader) {
        return false;
    }

    const auto &filepath = _local->get_filepath();

    auto spotifyTrack = _downloader->get_spotify()->researchTags(
        {_local->get_name(), _local->get_artist(), _local->get_albumName(),
         filepath.stem(), _local->get_length()});

    if (!spotifyTrack || !spotifyTrack) {
        std::cerr
            << "Unable to retrieve meta data from SpotifyAPI. Will not add '"
            << filepath << "' to blacklist!" << std::endl;
        return false;
    }

    if (_downloader->initializeDiscogs()) {
        Discogs::DiscogsAPI::SearchParams params{};

        params.releaseTitle = spotifyTrack->get_albumName();
        params.artist = spotifyTrack->get_albumArtist();
        params.year = spotifyTrack->get_year();
        params.categories.insert(ISearchResult::SearchCategory::Album);

        auto releases = _downloader->get_discogs()->searchRelease(params);
        if (releases.empty()) {
            std::cerr << "Could not locate track details using DicogsAPI!"
                      << std::endl;
        } else {
            std::unordered_set<std::string> styles;
            for (auto &&release : releases) {
                for (auto &&style : release->get_styles()) {
                    styles.insert(style);
                }
            }
            if (styles.size() == 1) {
                spotifyTrack->set_genre(*styles.begin());
            } else {
                std::cout << "Cannot determine correct genre:" << std::endl;
                for (auto &&style : styles) {
                    std::cout << style << std::endl;
                }
            }
        }
    } else {
        std::cerr << "DiscogsAPI unavailable. Cannot determine genre!"
                  << std::endl;
    }

    _local->applyDifferences(spotifyTrack);

    const auto &id = spotifyTrack->get_id();
    if (filepath.stem() != id) {
        _downloader->removeBlocked(_local);
        renameTrack(_local, id);
    }
    _downloader->makeBlocked(_local);

    return true;
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

void LocalTrackService::load(std::shared_ptr<IMediaEntity> _obj) {
    if (auto local = std::dynamic_pointer_cast<LocalTrack>(_obj))
        loadAdditionalData(local);
}
void LocalTrackService::loadAdditionalData(std::shared_ptr<LocalTrack> _local) {
    std::cerr << "Not yet implemented!" << std::endl;
}