#include "Services/MediaServiceRegistry.h"
#include "Discogs/DiscogsAPI.h"
#include "Spotify/SpotifyAPI.h"
#include "YouTube/YouTubeAPI.h"

MediaServiceRegistry::MediaServiceRegistry()
    : spotify(nullptr), discogs(nullptr), local(nullptr), youtube(nullptr),
      discogsSource(nullptr), spotifySource(nullptr), youtubeSource(nullptr),
      searchService(nullptr) {
    loadOrCreateConfig();
    initialize();
}

Spotify::SpotifyAPI *MediaServiceRegistry::get_spotify() {
    return spotify.get();
}
Discogs::DiscogsAPI *MediaServiceRegistry::get_discogs() {
    return discogs.get();
}
LocalTrackService *MediaServiceRegistry::get_local() { return local.get(); }
SearchService *MediaServiceRegistry::get_searchService() {
    return searchService.get();
}
YouTube::YouTubeAPI *MediaServiceRegistry::get_youtube() {
    return youtube.get();
}

const std::filesystem::path &MediaServiceRegistry::get_trackPath() const {
    return trackPath;
}
void MediaServiceRegistry::set_trackPath(const std::filesystem::path &_path) {
    trackPath = _path;
    if (local) {
        local->set_trackPath(_path);
    }
}

bool MediaServiceRegistry::initialize() {
    return initializeSpotify() && initializeYouTube() && initializeDiscogs() &&
           initializeLocal() && initializeSearchService();
}

bool MediaServiceRegistry::initializeSpotify(const std::string &_id,
                                             const std::string &_secret) {
    if (spotify)
        return true;
    if (_id.empty() && _secret.empty())
        initializeService(spotify, "Spotify");
    else
        initializeService(spotify, "Spotify", _id, _secret);

    if (spotify) {
        spotifySource = std::make_unique<SpotifyMediaSource>(*spotify);
        sources.push_back(spotifySource.get());
        return true;
    }
    return false;
}

bool MediaServiceRegistry::initializeYouTube(const std::string &_token) {
    if (youtube)
        return true;
    if (_token.empty())
        initializeService(youtube, "YouTube");
    else
        initializeService(youtube, "YouTube", _token);

    if (youtube) {
        youtubeSource = std::make_unique<YoutubeMediaSource>(*youtube);
        sources.push_back(youtubeSource.get());
        return true;
    }
    return false;
}

bool MediaServiceRegistry::initializeDiscogs(const std::string &_token) {
    if (discogs)
        return true;
    if (_token.empty())
        initializeService(discogs, "Discogs");
    else
        initializeService(discogs, "Discogs", _token);

    if (discogs) {
        discogsSource = std::make_unique<DiscogsMediaSource>(*discogs);
        sources.push_back(discogsSource.get());
        return true;
    }
    return false;
}

bool MediaServiceRegistry::initializeLocal() {
    return (local = std::make_unique<LocalTrackService>(trackPath)) != nullptr;
}

bool MediaServiceRegistry::isInitialized() const {
    return (spotify && youtube && discogs && local);
}

bool MediaServiceRegistry::initializeSearchService() {
    searchService = std::make_unique<SearchService>(sources);
    return true;
}

bool MediaServiceRegistry::loadOrCreateConfig() {
    const std::filesystem::path path = "config.json";

    if (!std::filesystem::exists(path)) {
        json config = {
            {"trackPath", std::filesystem::current_path() / "music/"}};
        std::ofstream file(path);
        file << config.dump(4);
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open config.json!" << std::endl;
        return false;
    }

    json config = json::parse(file);

    if (config.contains("trackPath") && config.at("trackPath").is_string()) {
        trackPath = config.at("trackPath").get<std::string>();
    } else {
        std::cerr << "Could not set custom trackPath" << std::endl;
    }

    return true;
}

bool MediaServiceRegistry::writeConfig() const {
    std::ofstream configFile("config.json");
    if (!configFile.is_open()) {
        std::cerr << "Could not write config.json!" << std::endl;
        return false;
    }
    json config = {{"trackPath", trackPath.string()}};
    configFile << config.dump(4);
    return true;
}