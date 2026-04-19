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

    using Callback = std::function<void()>;

    void loadTracks(const std::filesystem::path &_path, Downloader *_downloader,
                    bool recursiveSearch = false, bool includeBlocked = true,
                    bool includeNonBlocked = false);
    std::vector<size_t> getNextChunk(size_t count);
    std::shared_ptr<TrackInterface> getTrack(size_t idx);

    void setCallback(Callback cb) { onUpdate = std::move(cb); }
    void setSearchQuery(const std::string &_query);
    static bool matchesSearch(std::shared_ptr<TrackInterface> _track,
                              const std::string &_query);
    std::size_t getFilteredCount();

  private:
    std::vector<std::shared_ptr<TrackInterface>> allTracks;
    std::vector<size_t> filteredIndices;

    std::mutex tracksMutex;
    std::mutex filterMutex;

    std::thread worker;
    std::atomic<bool> stopRequested{false};
    std::atomic<size_t> generation{0};

    Callback onUpdate;

    size_t currentOffset = 0;
    std::string currentQuery;

    static std::string toLower(std::string _s);
};

#endif // LOCAL_TRACK_SERVICE_H
