#pragma once

#include <algorithm>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "Services/IMediaService.hpp"

class Downloader;
class LocalTrack;
class ITrack;
class LocalTrackService : public IMediaService {

  public:
    LocalTrackService() = default;
    ~LocalTrackService();

    size_t getGeneration();

    void loadTracks(const std::filesystem::path &_path, Downloader *_downloader,
                    bool recursiveSearch = false);

    std::function<void(std::vector<std::shared_ptr<ITrack>>, size_t)> onBatch;
    void dispatchBatch(std::vector<std::shared_ptr<ITrack>> &_batch,
                       size_t _gen);

    static bool renameTrack(std::shared_ptr<LocalTrack> _local,
                            const std::string &_filename);
    static bool deleteTrack(std::shared_ptr<LocalTrack> _local);

    static bool verify(std::shared_ptr<LocalTrack> _local,
                       Downloader *_downloader);

    void load(std::shared_ptr<IMediaEntity> _obj) override;
    void loadAdditionalData(std::shared_ptr<LocalTrack> _local);

  private:
    std::thread worker;
    std::atomic<bool> stopRequested{false};
    std::atomic<size_t> generation{0};
};
