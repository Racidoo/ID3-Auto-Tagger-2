#include "Services/DownloadService.h"
#include "Interfaces/ITrack.h"
#include "Local/LocalTrack.h"
#include "Local/LocalTrackService.h"
#include "Services/TagService.h"

bool DownloadService::downloadAudio(
    std::shared_ptr<ITrack> _track, const std::string &_youtubeId,
    const std::filesystem::path &_filepath,
    LocalTrackService *_localTrackService,
    std::function<void(int)> _onProgressCallback) {

    std::thread([_youtubeId, _onProgressCallback, _filepath, _track,
                 _localTrackService]() mutable {
        _onProgressCallback(0);
        // auto bestMatch = youTube->findBestMatch(_track, _onProgress);
        // if (bestMatch.empty()) {
        //     _onProgress(-1);
        //     return;
        // }
        _onProgressCallback(10);

        // std::filesystem::path trackName =
        //     trackPath / (_track->get_id() + ".mp3");

        // download resource
        std::string command =
            "yt-dlp -f bestaudio --extract-audio --audio-format 'mp3' "
            "--audio-quality 0 -o '" +
            _filepath.string() +
            "' https://www.youtube.com/watch?v=" + _youtubeId + " 2>&1";

        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe) {
            _onProgressCallback(-1);
            return false;
        }

        // Read output one character at a time to catch lines ending
        // in '\n' or
        // '\r'
        std::string lineBuffer;
        char ch;
        int preparationpercent(10);
        while (fread(&ch, 1, 1, pipe) == 1) {
            if (ch == '\n' || ch == '\r') {
                if (!lineBuffer.empty()) {
                    // Process the full line (either full message or
                    // progress line) std::cout << lineBuffer <<
                    // std::endl;
                    std::smatch match;
                    std::regex preparationRegex(R"(\[youtube\]\s+(\w*):.*)");
                    std::regex percentRegex(R"(\[download\]\s+(\d+\.\d+)%.*)");
                    if (std::regex_search(lineBuffer, match,
                                          preparationRegex)) {
                        preparationpercent++;
                        _onProgressCallback(preparationpercent); // Progress
                                                                 // percentage
                    } else if (std::regex_search(lineBuffer, match,
                                                 percentRegex)) {
                        int percent =
                            preparationpercent +
                            (std::stof(match[1].str()) / 100.0f) * 80.0f;
                        _onProgressCallback(percent); // Progress percentage
                    }

                    lineBuffer.clear();
                }
            } else {
                lineBuffer += ch;
            }
        }

        int status = pclose(pipe);
        if (status != 0) {
            _onProgressCallback(-1);
            return false;
        }

        auto localTrack(
            std::make_shared<LocalTrack>(_filepath, _localTrackService));
        // get additianl tags that are not relevant in normal search
        // _track->ensureLoaded(*spotify);
        // localTrack->applyDifferences(_track);
        TagService::applyDifferences(localTrack, _track);
        _onProgressCallback(99);
        _localTrackService->makeBlocked(localTrack);
        _onProgressCallback(100);
        return true;
    }).detach();
    return true;
}