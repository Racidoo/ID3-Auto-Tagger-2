#if !defined(LOCAL_TRACK_SERVICE_H)
#define LOCAL_TRACK_SERVICE_H

#include <algorithm>
#include <filesystem>
#include <mutex>
#include <string>
#include <thread>

#include "TrackInterface.h"

class Downloader;
class LocalTrackService {

  public:
    LocalTrackService() = default;
    ~LocalTrackService();

    size_t getGeneration();

    void loadTracks(const std::filesystem::path &_path, Downloader *_downloader,
                    bool recursiveSearch = false);

    std::function<void(std::vector<std::shared_ptr<TrackInterface>>, size_t)>
        onBatch;
    void dispatchBatch(std::vector<std::shared_ptr<TrackInterface>> &_batch,
                       size_t _gen);

  private:
    std::thread worker;
    std::atomic<bool> stopRequested{false};
    std::atomic<size_t> generation{0};
};

#endif // LOCAL_TRACK_SERVICE_H
