#include "../include/LocalTrackService.h"
#include "../include/Downloader.h"
#include "../include/LocalTrack.h"

LocalTrackService::~LocalTrackService() {
    if (worker.joinable())
        worker.join();
}

size_t LocalTrackService::getGeneration() { return generation; }

void LocalTrackService::loadTracks(const std::filesystem::path &_path,
                                   Downloader *_downloader,
                                   bool recursiveSearch, bool includeBlocked,
                                   bool includeNonBlocked) {
    stopRequested = true;

    if (worker.joinable())
        worker.join();

    stopRequested = false;

    size_t myGen = ++generation;

    worker = std::thread([this, myGen, _downloader, _path, recursiveSearch,
                          includeBlocked, includeNonBlocked]() {
        std::vector<std::shared_ptr<TrackInterface>> batch;
        batch.reserve(128);

        std::error_code ec;

        for (std::filesystem::recursive_directory_iterator it(_path, ec), end;
             it != end; it.increment(ec)) {

            if (stopRequested || myGen != generation)
                return;

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

            auto track =
                TrackInterface::fromLocal(std::make_shared<LocalTrack>(file));

            bool blocked = _downloader->isBlocked(track->get_id());
            track->set_inBlocklist(blocked);

            if (!includeBlocked && blocked)
                continue;
            if (!includeNonBlocked && !blocked)
                continue;

            batch.push_back(track);

            if (batch.size() >= 128) {
                dispatchBatch(batch, myGen);
                batch.clear();
            }
        }

        if (!batch.empty()) {
            dispatchBatch(batch, myGen);
        }
    });
}

void LocalTrackService::dispatchBatch(
    std::vector<std::shared_ptr<TrackInterface>> &_batch, size_t _gen) {
    if (!onBatch)
        return;

    // Move batch to avoid copy
    auto movedBatch = std::move(_batch);
    onBatch(std::move(movedBatch), _gen);
}
