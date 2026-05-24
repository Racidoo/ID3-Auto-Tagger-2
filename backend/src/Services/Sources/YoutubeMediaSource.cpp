#include "Services/Sources/YoutubeMediaSource.h"
#include "YouTube/YouTubeAPI.h"

YoutubeMediaSource::YoutubeMediaSource(YouTube::YouTubeAPI &_youtube)
    : youtube(_youtube) {}

IMediaService::MediaSourceId YoutubeMediaSource::get_id() const {
    return youtube.get_id();
}

ISearchResult YoutubeMediaSource::search(const std::string &_query,
                                         const SearchOptions &_options) {
    ISearchResult results;
    // if (_options.categories.find(ISearchResult::SearchCategory::Track) !=
    //     _options.categories.end()) {
    // Youtube::YoutubeAPI::SearchParams params{};

    // params.query = _query;
    // params.releaseTitle = _options->get_albumName();
    // params.artist = _options->get_albumArtist();
    // params.year = _options->get_year();
    // params.categories.insert(ISearchResult::SearchCategory::Album);
    results.videos = youtube.searchVideo(_query, nullptr, _options.limit);
    // results.tracks =
    //     youtube.search(_query, _options.market, _options.limit);
    // }
    return results;
}

ISearchResult YoutubeMediaSource::search(std::shared_ptr<ITrack> _track,
                                         const SearchOptions &_options) {}

bool YoutubeMediaSource::supports(const std::string &_input) const {
    std::smatch matches;
    std::string type, id;
    if (std::regex_search(_input, matches, pattern)) {
        if (matches[1].matched && matches[2].matched) {
            std::cout << "Youtube match: " << matches[1].str() << ", "
                      << matches[2].str() << std::endl;
            return true;
        }
    }
    std::cout << "no match" << std::endl;
    return false;
}

ISearchResult YoutubeMediaSource::resolve(const std::string &_input) {
    ISearchResult result;
    std::smatch matches;
    std::string type, id;
    if (std::regex_search(_input, matches, pattern)) {
        if (matches[1].matched && matches[2].matched) {
            type = matches[1].str();
            id = matches[2].str();
        }
    }

    // if (type == "track")
    //     result.tracks.push_back(getTrack(id));
    // else if (type == "album")
    //     result.albums.push_back(getAlbum(id));

    return result;
}

std::shared_ptr<IAlbum> YoutubeMediaSource::getAlbum(const std::string &_id) {
    static_assert("Not supported by Youtube-API!");
    // return youtube.getRelease(std::stoi(_id));
}
std::shared_ptr<IArtist> YoutubeMediaSource::getArtist(const std::string &_id) {
    static_assert("Not supported by Youtube-API!");
    // return youtube.getArtist(_id);
}
std::shared_ptr<ITrack> YoutubeMediaSource::getTrack(const std::string &_id) {
    static_assert("Not supported by Youtube-API!");
    // return youtube.getReleaseTrack(_id);
}