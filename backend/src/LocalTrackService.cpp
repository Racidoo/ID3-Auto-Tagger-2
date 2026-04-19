#include "../include/LocalTrackService.h"
#include "../include/Downloader.h"
#include "../include/LocalTrack.h"

LocalTrackService::~LocalTrackService() {
    if (worker.joinable())
        worker.join();
}

std::shared_ptr<TrackInterface> LocalTrackService::getTrack(size_t idx) {
    std::lock_guard<std::mutex> lock(tracksMutex);
    return allTracks[idx];
}

std::vector<size_t> LocalTrackService::getNextChunk(size_t count) {
    size_t end;

    {
        std::lock_guard<std::mutex> lock(tracksMutex);
        end = std::min(currentOffset + count, allTracks.size());
    }

    std::vector<size_t> indices;
    indices.reserve(end - currentOffset);

    for (size_t i = currentOffset; i < end; ++i)
        indices.push_back(i);

    currentOffset = end;
    return indices;
}

void LocalTrackService::loadTracks(const std::filesystem::path &_path,
                                   Downloader *_downloader,
                                   bool recursiveSearch, bool includeBlocked,
                                   bool includeNonBlocked) {
    stopRequested = true;

    if (worker.joinable())
        worker.join();

    stopRequested = false;

    size_t myGen = ++generation;

    {
        std::lock_guard<std::mutex> lock(tracksMutex);
        std::lock_guard<std::mutex> fLock(filterMutex);
        allTracks.clear();
        filteredIndices.clear();
        currentOffset = 0;
    }

    worker = std::thread([this, myGen, _downloader, _path, recursiveSearch,
                          includeBlocked, includeNonBlocked]() {
        size_t notifyCounter = 0;

        std::error_code ec;
        auto lastTime = std::chrono::high_resolution_clock::now();

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

            auto track = TrackInterface::fromLocal(LocalTrack(file));

            bool blocked = _downloader->isBlocked(track->get_id());
            track->set_inBlocklist(blocked);

            if (!includeBlocked && blocked)
                continue;
            if (!includeNonBlocked && !blocked)
                continue;
            {
                std::lock_guard<std::mutex> lock(tracksMutex);
                allTracks.push_back(track);

                std::lock_guard<std::mutex> fLock(filterMutex);
                filteredIndices.push_back(allTracks.size());
            }

            // batching
            if (++notifyCounter >= 128) {
                notifyCounter = 0;

                if (onUpdate)
                    onUpdate(); // worker thread
            }
        }
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - lastTime);
        {
            std::lock_guard<std::mutex> lock(tracksMutex);
            std::cout << "Found " << allTracks.size() << " files in "
                      << time.count() << " milliseconds" << std::endl;
        }

        // final notify
        if (onUpdate)
            onUpdate();
    });
}

std::size_t LocalTrackService::getFilteredCount() {
    std::lock_guard<std::mutex> lock(filterMutex);
    return filteredIndices.size();
}

std::string LocalTrackService::toLower(std::string _s) {
    std::transform(_s.begin(), _s.end(), _s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return _s;
};

bool LocalTrackService::matchesSearch(std::shared_ptr<TrackInterface> _track,
                                      const std::string &_query) {

    if (_query.empty())
        return true;

    // Normalize query
    std::string query = toLower(_query);

    // --- Filename ---
    std::string filename = toLower(_track->get_id()); // assume exists

    if (filename.find(query) != std::string::npos)
        return true;

    auto contains = [&](const std::string &_str) {
        return toLower(_str).find(query) != std::string::npos;
    };

    return contains(_track->get_title()) || contains(_track->get_artist()) ||
           contains(_track->get_album()) || contains(_track->get_genre());
}

void LocalTrackService::setSearchQuery(const std::string &_query) {
    std::lock_guard<std::mutex> lock(tracksMutex);
    std::lock_guard<std::mutex> fLock(filterMutex);

    currentQuery = _query;
    filteredIndices.clear();
    currentOffset = 0;

    for (size_t i = 0; i < allTracks.size(); ++i) {
        if (_query.empty() || matchesSearch(allTracks[i], _query)) {
            filteredIndices.push_back(i);
        }
    }
}
