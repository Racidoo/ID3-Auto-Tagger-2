#pragma once

#include <filesystem>
#include <functional>
#include <future>
#include <memory>
#include <regex>
#include <string>

class ITrack;
class LocalTrackService;
class DownloadService {
  public:
    DownloadService() = delete;
    ~DownloadService() = delete;

    static bool
    downloadAudio(std::shared_ptr<ITrack> _track, const std::string &_youtubeId,
                  const std::filesystem::path &_filepath,
                  LocalTrackService *_localTrackService,
                  std::function<void(int)> _onProgressCallback = {});
};