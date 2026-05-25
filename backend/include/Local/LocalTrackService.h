#pragma once

#include <algorithm>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "Services/IMediaService.hpp"

class ITrack;
class LocalTrack;
class TrackVerificationIndex;

class LocalTrackService : public IMediaService {

  public:
    LocalTrackService(const std::filesystem::path &_trackPath,
                      TrackVerificationIndex &_index);
    ~LocalTrackService();

    LocalTrackService(const LocalTrackService &) = delete;
    LocalTrackService &operator=(const LocalTrackService &) = delete;
    LocalTrackService(LocalTrackService &&) = delete;
    LocalTrackService &operator=(LocalTrackService &&) = delete;

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

  private:
    std::filesystem::path trackPath;

    TrackVerificationIndex &verificationIndex;
    std::thread worker;
    std::atomic<bool> stopRequested;
    std::atomic<size_t> generation;
};
