#include "Services/Sources/DiscogsMediaSource.h"

DiscogsMediaSource::DiscogsMediaSource(Discogs::DiscogsAPI &_discogs)
    : discogs(_discogs) {}

IMediaService::MediaSourceId DiscogsMediaSource::get_id() const {
    return discogs.get_id();
}

ISearchResult DiscogsMediaSource::search(const std::string &_query,
                                         const SearchOptions &_options) {
    ISearchResult results;
    Discogs::DiscogsAPI::SearchParams params{};

    params.query = _query;
    params.categories = _options.categories;

    if (_options.categories.find(ISearchResult::SearchCategory::Album) !=
        _options.categories.end()) {
        results.albums = discogs.searchRelease(params, _options.limit);
    }
    if (_options.categories.find(ISearchResult::SearchCategory::Track) !=
        _options.categories.end()) {
        results = discogs.searchReleaseTrack(params, _options.limit);
    }
    return results;
}

bool DiscogsMediaSource::supports(const std::string &_input) const {
    std::smatch matches;
    std::string type, id;
    if (std::regex_search(_input, matches, pattern)) {
        if (matches[1].matched && matches[2].matched) {
            std::cout << "Discogs match: " << matches[1].str() << ", "
                      << matches[2].str() << std::endl;
            return true;
        }
    }
    std::cout << "no match" << std::endl;
    return false;
}

ISearchResult DiscogsMediaSource::resolve(const std::string &_input) {
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

std::shared_ptr<IAlbum> DiscogsMediaSource::getAlbum(const std::string &_id) {
    return discogs.getRelease(std::stoi(_id));
}
std::shared_ptr<IArtist> DiscogsMediaSource::getArtist(const std::string &_id) {
    static_assert("Not supported by Discogs-API!");
    // return discogs.getArtist(_id);
}
std::shared_ptr<ITrack> DiscogsMediaSource::getTrack(const std::string &_id) {
    static_assert("Not supported by Discogs-API!");
    return nullptr;
}