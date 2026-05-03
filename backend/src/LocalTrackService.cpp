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
                                   bool recursiveSearch) {
    stopRequested = true;

    if (worker.joinable())
        worker.join();

    stopRequested = false;

    size_t myGen = ++generation;

    worker = std::thread([this, myGen, _downloader, _path, recursiveSearch]() {
        std::vector<std::shared_ptr<TrackInterface>> batch;
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

            auto track =
                TrackInterface::fromLocal(std::make_shared<LocalTrack>(file));

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
    std::vector<std::shared_ptr<TrackInterface>> &_batch, size_t _gen) {
    if (!onBatch)
        return;

    // Move batch to avoid copy
    auto movedBatch = std::move(_batch);
    onBatch(std::move(movedBatch), _gen);
}
