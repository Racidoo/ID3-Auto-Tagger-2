#include "Services/Sources/SpotifyMediaSource.h"

SpotifyMediaSource::SpotifyMediaSource(Spotify::SpotifyAPI &_spotify)
    : spotify(_spotify) {}

IMediaService::MediaSourceId SpotifyMediaSource::get_id() const {
    return spotify.get_id();
}

ISearchResult SpotifyMediaSource::search(const std::string &_query,
                                         const SearchOptions &_options) {
    ISearchResult results;
    if (_options.categories.find(ISearchResult::SearchCategory::Track) !=
        _options.categories.end()) {
        results.tracks =
            spotify.searchTrack(_query, _options.market, _options.limit);
    }
    if (_options.categories.find(ISearchResult::SearchCategory::Album) !=
        _options.categories.end()) {
        results.albums =
            spotify.searchAlbum(_query, _options.market, _options.limit);
    }
    if (_options.categories.find(ISearchResult::SearchCategory::Artist) !=
        _options.categories.end()) {
        results.artists =
            spotify.searchArtist(_query, _options.market, _options.limit);
    }
    if (_options.categories.find(ISearchResult::SearchCategory::Playlist) !=
        _options.categories.end()) {
        results.playlists =
            spotify.searchPlaylist(_query, _options.market, _options.limit);
    }
    return results;
}

ISearchResult SpotifyMediaSource::search(std::shared_ptr<ITrack> _track,
                                         const SearchOptions &_options) {
    ISearchResult results;
    SearchQuery searchQuery(_track);
    if (_options.categories.find(ISearchResult::SearchCategory::Track) !=
        _options.categories.end()) {
        std::string stringQuery;
        if (searchQuery.title.has_value()) {
            stringQuery += " track:" + searchQuery.title.value();
        }
        if (searchQuery.artist.has_value()) {
            stringQuery += " artist:" + searchQuery.artist.value();
        }
        if (searchQuery.album.has_value()) {
            stringQuery += " album:" + searchQuery.album.value();
        }
        if (stringQuery.empty()) {
            stringQuery = searchQuery.filename.value_or("");
        }
        results.tracks =
            spotify.searchTrack(stringQuery, _options.market, _options.limit);
        // fallback if search was too explicit
        if (results.tracks.empty()) {
            stringQuery = searchQuery.title.value_or("") +
                          searchQuery.artist.value_or("");
            results.tracks = spotify.searchTrack(stringQuery, _options.market,
                                                 _options.limit);
        }
    }
    return results;
}

bool SpotifyMediaSource::supports(const std::string &_input) const {
    std::smatch matches;
    std::string type, id;
    if (std::regex_search(_input, matches, pattern)) {
        if (matches[1].matched && matches[2].matched) {
            std::cout << "Spotify match: " << matches[1].str() << ", "
                      << matches[2].str() << std::endl;
            return true;
        }
    }
    std::cout << "no match" << std::endl;
    return false;
}

ISearchResult SpotifyMediaSource::resolve(const std::string &_input) {
    ISearchResult result;
    std::smatch matches;
    std::string type, id;
    if (std::regex_search(_input, matches, pattern)) {
        if (matches[1].matched && matches[2].matched) {
            type = matches[1].str();
            id = matches[2].str();
        }
    }

    if (type == "track")
        result.tracks.push_back(getTrack(id));
    else if (type == "album")
        result.albums.push_back(getAlbum(id));

    return result;
}

std::shared_ptr<IAlbum> SpotifyMediaSource::getAlbum(const std::string &_id) {
    return spotify.getAlbum(_id);
}
std::shared_ptr<IArtist> SpotifyMediaSource::getArtist(const std::string &_id) {
    return spotify.getArtist(_id);
}
std::shared_ptr<ITrack> SpotifyMediaSource::getTrack(const std::string &_id) {
    return spotify.getTrack(_id);
}