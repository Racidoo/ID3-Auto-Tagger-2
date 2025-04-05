#include "../include/SpotifyAPI.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

#include <iostream>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>

// int main() {

// SpotifyAPI spotify;
// if (spotify.getTrack("6tGtFeIO3yYuVvNLAdHU0A")
//         .writeID3V2Tags("6tGtFeIO3yYuVvNLAdHU0A.mp3"))
//     std::cout << "Successfully updated tags" << std::endl;

//     return 0;
// }

#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include "../include/Downloader.h"
#include "../include/YouTube.h"

int main() {

    Downloader downloader();
    std::string url = "https://open.spotify.com/intl-de/track/"
                      "4CYD6cjV66A13BPOYIiEgm?si=5f85981329b84fef";

    downloader.downloadResource(url);
    // SpotifyAPI spotify;
    // YouTube youTube;

    // std::string id = "0agQ9vIV7NP4dntGKLcCXO";

    // std::string bestMatch = youTube.findBestMatch(spotify.getTrack(id));

    // std::string command = "yt-dlp -f bestaudio --extract-audio --audio-format
    // "
    //                       "mp3 --audio-quality 0 -o '" +
    //                       id +
    //                       ".mp3' https://www.youtube.com/watch?v=" +
    //                       bestMatch;
    // std::system(command.c_str());
    std::cout << command << std::endl;

    return 0;
}
