#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

using json = nlohmann::json;

#include "Services/IMediaService.hpp"

class LocalTrack;
class ITrack;
class LocalTrackService : public IMediaService {

  public:
    LocalTrackService(const std::filesystem::path &_trackPath);
    ~LocalTrackService();

    inline constexpr IMediaService::MediaSourceId get_id() const override {
        return MediaSourceId::Local;
    }

    size_t getGeneration();
    const std::filesystem::path &get_trackPath() const;
    void set_trackPath(const std::filesystem::path &_path);

    void loadTracks(bool recursiveSearch = false);

    std::function<void(std::vector<std::shared_ptr<ITrack>>, size_t)> onBatch;
    void dispatchBatch(std::vector<std::shared_ptr<ITrack>> &_batch,
                       size_t _gen);

    static bool renameTrack(std::shared_ptr<LocalTrack> _local,
                            const std::string &_filename);
    static bool deleteTrack(std::shared_ptr<LocalTrack> _local);

    bool load(std::shared_ptr<IMediaEntity> _obj) override;
    bool loadAdditionalData(std::shared_ptr<LocalTrack> _local);

    bool isBlocked(std::shared_ptr<ITrack> _data) const;
    void makeBlocked(std::shared_ptr<ITrack> _data);
    void removeBlocked(std::shared_ptr<ITrack> _data);

  private:
    bool loadOrCreateBlacklist();
    bool writeBlacklist() const;

    std::filesystem::path trackPath;

    json blacklist;
    std::thread worker;
    std::atomic<bool> stopRequested;
    std::atomic<size_t> generation;
};
