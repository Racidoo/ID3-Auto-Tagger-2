#include "Services/TagService.h"
#include "Discogs/ReleaseTrack.h"
// #include "Services/TagService.h"
#include "Spotify/SpotifyAPI.h"
#include "Spotify/Track.h"

std::vector<std::string> TagService::collaborationSeparators = {
    ",",     "&",           " x ",    " X ", " feat. ",
    " ft. ", " featuring ", " with ", "+",   "/"};

const std::unordered_map<IMediaService::MediaSourceId, TagService::SourcePolicy>
    TagService::policies = {
        {IMediaService::MediaSourceId::Discogs,
         {{},
          {TagField::Title, TagField::Artist, TagField::Album,
           TagField::AlbumArtist, TagField::AlbumCover, TagField::Genre,
           TagField::Copyright, TagField::Year, TagField::Label,
           TagField::TrackNumber, TagField::DiscNumber}}},
        {IMediaService::MediaSourceId::Local,
         {{TagField::Title, TagField::Artist, TagField::Album,
           TagField::AlbumArtist, TagField::AlbumCover, TagField::Genre,
           TagField::Copyright, TagField::Year, TagField::Label,
           TagField::TrackNumber, TagField::DiscNumber},
          {}}},
        {IMediaService::MediaSourceId::Spotify,
         {{TagField::Title, TagField::Artist, TagField::Album,
           TagField::AlbumArtist, TagField::AlbumCover, TagField::Year,
           TagField::TrackNumber, TagField::DiscNumber, TagField::Copyright,
           TagField::Label},
          {TagField::Genre}}},
};

const std::vector<std::unique_ptr<TagService::ITrackFieldDescriptor>>
    TagService::trackFields = [] {
        std::vector<std::unique_ptr<ITrackFieldDescriptor>> fields;

        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Title, "title",
                [](std::shared_ptr<ITrack> t) { return t->get_name(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) { t->set_name(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Artist, "artist",
                [](std::shared_ptr<ITrack> t) { return t->get_artist(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) { t->set_artist(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Album, "album",
                [](std::shared_ptr<ITrack> t) { return t->get_albumName(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) {
                    t->set_albumName(v);
                }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::AlbumArtist, "albumArtist",
                [](std::shared_ptr<ITrack> t) { return t->get_albumArtist(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) {
                    t->set_albumArtist(v);
                }));
        fields.push_back(
            std::make_unique<
                TrackFieldDescriptor<std::optional<std::vector<std::byte>>>>(
                TagField::AlbumCover, "albumCover",
                [](std::shared_ptr<ITrack> t) { return t->get_image(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::vector<std::byte>> &v) {
                    t->set_image(v);
                }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Genre, "genre",
                [](std::shared_ptr<ITrack> t) { return t->get_genre(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) { t->set_genre(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Copyright, "copyright",
                [](std::shared_ptr<ITrack> t) { return t->get_copyright(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) {
                    t->set_copyright(v);
                }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<size_t>>>(
                TagField::Year, "year",
                [](std::shared_ptr<ITrack> t) { return t->get_year(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<size_t> &v) { t->set_year(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<std::string>>>(
                TagField::Label, "label",
                [](std::shared_ptr<ITrack> t) { return t->get_label(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<std::string> &v) { t->set_label(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<size_t>>>(
                TagField::TrackNumber, "trackNumber",
                [](std::shared_ptr<ITrack> t) { return t->get_trackNumber(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<size_t> &v) { t->set_trackNumber(v); }));
        fields.push_back(
            std::make_unique<TrackFieldDescriptor<std::optional<size_t>>>(
                TagField::DiscNumber, "discNumber",
                [](std::shared_ptr<ITrack> t) { return t->get_discNumber(); },
                [](std::shared_ptr<ITrackMutable> t,
                   const std::optional<size_t> &v) { t->set_discNumber(v); }));
        return fields;
    }();

TagService::TagResearchResult
TagService::identifyTrack(std::shared_ptr<ITrackMutable> _track,
                          SearchService *_searchService) {
    TagResearchResult result;
    SearchOptions options;

    options.sources.insert(IMediaService::MediaSourceId::Spotify);
    options.sources.insert(IMediaService::MediaSourceId::Discogs);
    options.categories.insert(ISearchResult::SearchCategory::Track);

    _track->ensureLoaded();

    auto searchResults = _searchService->search(_track, options);

    auto merged = merge(_track, searchResults);

    if (merged.sources.empty()) {
        std::cerr << "Unable to fetch additional meta-data!" << std::endl;
        return {};
    }
    std::pair<const double, std::shared_ptr<ITrack>> *highestScore = nullptr;
    std::vector<VerificationCandidate> topCandiates;
    for (auto &&source : merged.sources) {
        if (Spotify::SpotifyAPI::isValidIdFormat(source.second->get_id())) {
            if (!highestScore || highestScore->first < source.first) {
                highestScore = &source;
            }
        }
        if (source.first >= USER_VERIFY_THRESHOLD) {
            topCandiates.push_back({source.first, source.second, {}});
        }
    }

    if (!highestScore) {
        result.state = VerificationState::Rejected;
        return result;
    }
    if (highestScore->first >= AUTO_ACCEPT_THRESHOLD) {
        result.state = VerificationState::Accepted;
        result.acceptedTrack = highestScore->second;
    } else if (highestScore->first >= USER_VERIFY_THRESHOLD) {
        result.state = VerificationState::RequiresUserVerification;
        result.candidates = topCandiates;
    } else {
        result.state = VerificationState::Rejected;
    }

    return result;
}

std::string TagService::researchVideoId(std::shared_ptr<ITrack> _track,
                                        SearchService *_searchService) {
    SearchOptions options;

    options.sources.insert(IMediaService::MediaSourceId::YouTube);
    options.categories.insert(ISearchResult::SearchCategory::Video);

    _track->ensureLoaded();

    auto searchResults = _searchService->search(_track, options);

    auto merged = TagService::merge(_track, searchResults);

    if (merged.videos.empty()) {
        std::cerr << "Unable to fetch videos for " << _track->get_id() << "!"
                  << std::endl;
        return {};
    }

    return merged.videos.rbegin()->second->get_id();
}

void TagService::applyTagDifferences(std::shared_ptr<ITrackMutable> _current,
                                     std::shared_ptr<ITrack> _incoming) {

    auto it = policies.find(_incoming->get_mediaSourceId());
    if (it == policies.end()) {
        std::cerr << "[" << __FILE__ << ":" << __LINE__
                  << "]: Unsupported mediaSource!" << std::endl;
        return;
    }
    const auto &policy = it->second;

    for (const auto &desc : trackFields) {
        desc->apply(_current, _incoming, policy);
    }
}

std::vector<TagService::AggregatedTrack>
TagService::aggregate(const std::vector<ISearchResult> &_results) {
    std::vector<std::shared_ptr<ITrack>> allTracks;
    std::vector<std::shared_ptr<IVideo>> allVideos;

    for (const auto &result : _results) {
        for (const auto &track : result.tracks) {
            allTracks.push_back(track);
        }
        for (const auto &video : result.videos) {
            allVideos.push_back(video);
        }
    }
    std::vector<AggregatedTrack> aggregated;

    for (const auto &track : allTracks) {

        bool merged = false;

        for (auto &group : aggregated) {

            if (scoreTracks(track, group.primary) >= AUTO_ACCEPT_THRESHOLD) {
                group.sources.insert(
                    {scoreTracks(track, group.primary), track});
                merged = true;
                break;
            }
        }

        if (!merged) {
            AggregatedTrack group;
            group.primary = track;
            group.sources.insert({scoreTracks(track, group.primary), track});

            aggregated.push_back(group);
        }
    }

    return aggregated;
}

TagService::AggregatedTrack
TagService::merge(std::shared_ptr<ITrack> _master,
                  const std::vector<ISearchResult> &_results) {
    std::vector<std::shared_ptr<ITrack>> allTracks;
    std::vector<std::shared_ptr<IVideo>> allVideos;
    AggregatedTrack aggregated;

    aggregated.primary = _master;

    for (const auto &result : _results) {
        for (const auto &track : result.tracks) {
            allTracks.push_back(track);
        }
        for (const auto &video : result.videos) {
            allVideos.push_back(video);
        }
    }

    for (auto &&track : allTracks) {
        auto score = scoreTracks(track, aggregated.primary);
        auto it = aggregated.sources.insert({score, track});
    }
    for (auto &&video : allVideos) {
        auto score = scoreTracks(video, aggregated.primary);
        auto it = aggregated.videos.insert({score, video});
    }

    return aggregated;
}

double TagService::scoreTracks(const std::shared_ptr<ITrack> &_a,
                               const std::shared_ptr<ITrack> &_b) {

    auto nameScore = similarityScore(_a->get_name().value_or(""),
                                     _b->get_name().value_or("")) *
                     0.35;
    auto artistScore = similarityScore(_a->get_artist().value_or(""),
                                       _b->get_artist().value_or("")) *
                       0.35;
    auto albumNameScore = similarityScore(_a->get_albumName().value_or(""),
                                          _b->get_albumName().value_or("")) *
                          0.1;
    auto lengthScore =
        similarityScore(_a->get_length(), _b->get_length()) * 0.2;
    auto score = nameScore + artistScore + albumNameScore + lengthScore;

    std::cout << _a->get_id() << " == " << _b->get_id() << ": " << score
              << "\n\tname: " << _a->get_name().value_or("")
              << " == " << _b->get_name().value_or("") << ": " << nameScore
              << "\n\tartist: " << _a->get_artist().value_or("")
              << " == " << _b->get_artist().value_or("") << ": " << artistScore
              << "\n\talbumName: " << _a->get_albumName().value_or("")
              << " == " << _b->get_albumName().value_or("") << ": "
              << albumNameScore << "\n\tlength: " << _a->get_length()
              << " == " << _b->get_length() << ": " << lengthScore << std::endl;
    return score;
}

double TagService::scoreTracks(const std::shared_ptr<IVideo> &_a,
                               const std::shared_ptr<ITrack> &_b) {
    auto nameScore =
        static_cast<double>(findInString(_a->get_name().value_or(""),
                                         _b->get_name().value_or(""))) *
        0.4;
    auto artistScore = scoreArtistMatch(_a->get_name().value_or(""),
                                        _b->get_artist().value_or("")) *
                       0.4;
    auto lengthScore =
        similarityScore(_a->get_length(), _b->get_length()) * 0.2;
    auto score = nameScore + artistScore + lengthScore;

    std::cout << _a->get_id() << " == " << _b->get_id() << ": " << score
              << "\n\tname: " << _a->get_name().value_or("")
              << " == " << _b->get_name().value_or("") << ": " << nameScore
              << "\n\tartist: " << _a->get_name().value_or("")
              << " == " << _b->get_artist().value_or("") << ": " << artistScore
              << "\n\tlength: " << _a->get_length()
              << " == " << _b->get_length() << ": " << lengthScore << std::endl;
    return score;
}

// Convert string to lowercase
std::string TagService::toLower(const std::string &_str) {
    std::string result = _str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Compute Levenshtein distance
int TagService::levenshteinDistance(const std::string &_s1,
                                    const std::string &_s2) {
    size_t len1 = _s1.size(), len2 = _s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

    for (size_t i = 0; i <= len1; ++i)
        dp[i][0] = i;
    for (size_t j = 0; j <= len2; ++j)
        dp[0][j] = j;

    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            int cost = (_s1[i - 1] == _s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({dp[i - 1][j] + 1,          // Deletion
                                 dp[i][j - 1] + 1,          // Insertion
                                 dp[i - 1][j - 1] + cost}); // Substitution
        }
    }
    return dp[len1][len2];
}

// Calculate a similarity score
double TagService::similarityScore(const std::string &_s1,
                                   const std::string &_s2) {
    int dist = levenshteinDistance(toLower(_s1), toLower(_s2));
    int maxLength = std::max(_s1.size(), _s2.size());
    return 1.0 - (double)dist / maxLength; // Normalize to 0-1
}

// Function to compute similarity score between two durations
double TagService::similarityScore(int duration1, int duration2) {
    int diff = std::abs(duration1 - duration2);

    // If exact match, return 1.0
    if (diff == 0)
        return 1.0;

    // Exponential decay to reduce score as difference increases
    double decayFactor =
        0.05; // Controls how fast the score drops (adjust if needed)
    return std::exp(-decayFactor * diff);
}

double TagService::similarityScoreDate(const std::string &_date1,
                                       const std::string &_date2) {
    auto daysSinceEpoch = [](std::string date) -> int {
        if (date.size() == 4) {
            date.append("-01-01");
        } else if (date.find('-') == std::string::npos) {
            date.insert(6, "-");
            date.insert(4, "-");
        }
        struct std::tm tm = {};
        strptime(date.c_str(), "%Y-%m-%d", &tm);
        return std::mktime(&tm) / 86400; // Convert to days
    };
    return similarityScore(daysSinceEpoch(_date1), daysSinceEpoch(_date2));
}

bool TagService::findInString(const std::string &_base,
                              const std::string &_searchText) {
    return toLower(_base).find(toLower(_searchText)) != std::string::npos;
}

std::vector<std::string> TagService::splitArtists(std::string _artist) {
    auto normalize = [](std::string &_str) {
        std::transform(_str.begin(), _str.end(), _str.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        // trim
        auto notSpace = [](unsigned char c) { return !std::isspace(c); };

        _str.erase(_str.begin(),
                   std::find_if(_str.begin(), _str.end(), notSpace));

        _str.erase(std::find_if(_str.rbegin(), _str.rend(), notSpace).base(),
                   _str.end());
    };

    normalize(_artist);

    std::vector<std::string> artists = {_artist};

    for (const auto &separator : collaborationSeparators) {
        std::vector<std::string> split;

        for (const auto &artist : artists) {
            size_t start = 0;
            size_t pos = 0;

            while ((pos = artist.find(separator, start)) != std::string::npos) {
                auto token = artist.substr(start, pos - start);

                normalize(token);

                if (!token.empty())
                    split.push_back(token);

                start = pos + separator.size();
            }

            auto token = artist.substr(start);

            normalize(token);

            if (!token.empty())
                split.push_back(token);
        }

        artists = std::move(split);
    }

    // deduplicate
    std::sort(artists.begin(), artists.end());
    artists.erase(std::unique(artists.begin(), artists.end()), artists.end());

    return artists;
}

double TagService::scoreArtistMatch(const std::string &_title,
                                    const std::string &_artist) {

    auto artists = splitArtists(_artist);

    if (artists.empty())
        return 0.0;

    unsigned matches = 0;

    for (const auto &artist : artists) {
        if (findInString(_title, artist))
            ++matches;
    }

    return static_cast<double>(matches) / artists.size();
}

std::string TagService::normalizeTrackTitle(std::string _title) {

    auto toLower = [](std::string &_str) {
        std::transform(_str.begin(), _str.end(), _str.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    };

    auto trim = [](std::string &_str) {
        auto notSpace = [](unsigned char c) { return !std::isspace(c); };

        _str.erase(_str.begin(),
                   std::find_if(_str.begin(), _str.end(), notSpace));

        _str.erase(std::find_if(_str.rbegin(), _str.rend(), notSpace).base(),
                   _str.end());
    };

    auto eraseRegex = [&_title](const std::string &_pattern) {
        _title = std::regex_replace(
            _title, std::regex(_pattern, std::regex_constants::icase), "");
    };

    toLower(_title);

    // remove bracketed variants
    eraseRegex(R"(\(.*?(mix|remix|edit|version|extended|live).*?\))");
    eraseRegex(R"(\[.*?(mix|remix|edit|version|extended|live).*?\])");

    // remove trailing remix suffixes
    eraseRegex(R"(\s*-\s*.*?(mix|remix|edit|version|extended|live)$)");

    // remove common youtube garbage
    eraseRegex(R"(\(official.*?\))");
    eraseRegex(R"(\[official.*?\])");
    eraseRegex(R"(\(lyrics?\))");
    eraseRegex(R"(\[lyrics?\])");
    eraseRegex(R"(\(audio\))");
    eraseRegex(R"(\[audio\])");

    // collapse whitespace
    eraseRegex(R"(\s+)");

    trim(_title);

    return _title;
}