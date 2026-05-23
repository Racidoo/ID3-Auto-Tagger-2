#include "Services/TrackIdentificationService.h"
#include "Discogs/ReleaseTrack.h"
#include "Spotify/Track.h"

TrackIdentificationService::SearchQuery::SearchQuery(
    std::shared_ptr<ITrack> _track) {
    title = _track->get_name();
    album = _track->get_albumName();
    filename = _track->get_id();
    durationSeconds = _track->get_length();
}

std::shared_ptr<ITrack> TrackIdentificationService::findBestMatch(
    const SearchQuery &_query,
    const std::vector<std::shared_ptr<ITrack>> &_candidates) {

    double bestScore(0.0);
    std::vector<std::shared_ptr<ITrack>> topMatches;
    int num_res(0);
    double allScores(0.0);
    for (auto &&track : _candidates) {
        num_res++;
        double score(0.0);
        score += similarityScore(_query.title, track->get_name());
        score += similarityScore(_query.durationSeconds, track->get_length());

        if (!_query.album.empty()) {
            score += similarityScore(_query.album, track->get_albumName());
        }

        // Bonus, if album is single
        if (auto spotifyTrack =
                std::dynamic_pointer_cast<Spotify::Track>(track)) {
            if (spotifyTrack->get_album()->get_type() ==
                IAlbum::album_type_t::SINGLE) {
                score += 1;
            }
        }
        if (auto discogsTrack =
                std::dynamic_pointer_cast<Discogs::ReleaseTrack>(track)) {
            if (discogsTrack->get_release().lock()->get_type() ==
                IAlbum::album_type_t::SINGLE) {
                score += 1;
            }
        }
        std::cout << track->get_id() << "\tscore: " << score << std::endl;

        if (score > bestScore) {
            bestScore = score;
            topMatches.clear();
            topMatches.push_back(track);
        } else if (score == bestScore) {
            topMatches.push_back(track);
        }
        allScores += score;
    }
    double middle = allScores / num_res;

    std::cout << "Mean deviation: " << bestScore - middle << std::endl;

    if (bestScore < 1.4) {
        return nullptr;
    }

    return topMatches[0];
}

std::vector<TrackIdentificationService::AggregatedTrack>
TrackIdentificationService::aggregate(
    const std::vector<ISearchResult> &_results) {
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

            if (isSameTrack(track, group.primary)) {
                group.sources.push_back(track);
                merged = true;
                break;
            }
        }

        if (!merged) {
            AggregatedTrack group;
            group.primary = track;
            group.sources.push_back(track);

            aggregated.push_back(group);
        }
    }

    return aggregated;
}

TrackIdentificationService::AggregatedTrack
TrackIdentificationService::merge(std::shared_ptr<ITrack> _master,
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
        if (isSameTrack(track, aggregated.primary)) {
            aggregated.sources.push_back(track);
        } else {
            std::cout << track->get_id() << " is not similar to "
                      << aggregated.primary->get_id() << std::endl;
        }
    }
    for (auto &&video : allVideos) {
        if (isSameTrack(video, aggregated.primary)) {
            aggregated.videos.push_back(video);
        } else {
            std::cout << video->get_id() << " is not similar to "
                      << aggregated.primary->get_id() << std::endl;
        }
    }

    return aggregated;
}

bool TrackIdentificationService::isSameTrack(
    const std::shared_ptr<ITrack> &_a, const std::shared_ptr<ITrack> &_b) {
    return scoreTracks(_a, _b) >= 0.8;
}

bool TrackIdentificationService::isSameTrack(
    const std::shared_ptr<IVideo> &_a, const std::shared_ptr<ITrack> &_b) {
    return scoreTracks(_a, _b) >= 0.8;
}

double
TrackIdentificationService::scoreTracks(const std::shared_ptr<ITrack> &_a,
                                        const std::shared_ptr<ITrack> &_b) {

    auto nameScore = similarityScore(_a->get_name(), _b->get_name()) * 0.35;
    auto artistScore =
        similarityScore(_a->get_artist(), _b->get_artist()) * 0.35;
    auto albumNameScore =
        similarityScore(_a->get_albumName(), _b->get_albumName()) * 0.1;
    auto lengthScore =
        similarityScore(_a->get_length(), _b->get_length()) * 0.2;
    auto score = nameScore + artistScore + albumNameScore + lengthScore;

    std::cout << _a->get_id() << " == " << _b->get_id() << ": " << score
              << "\n\tname: " << _a->get_name() << " == " << _b->get_name()
              << ": " << nameScore << "\n\tartist: " << _a->get_artist()
              << " == " << _b->get_artist() << ": " << artistScore
              << "\n\talbumName: " << _a->get_albumName()
              << " == " << _b->get_albumName() << ": " << albumNameScore
              << "\n\tlength: " << _a->get_length()
              << " == " << _b->get_length() << ": " << lengthScore << std::endl;
    return score;
}

double
TrackIdentificationService::scoreTracks(const std::shared_ptr<IVideo> &_a,
                                        const std::shared_ptr<ITrack> &_b) {
    auto nameScore =
        static_cast<double>(findInString(_a->get_name(), _b->get_name())) * 0.4;
    auto artistScore = scoreArtistMatch(_a->get_name(), _b->get_artist()) * 0.4;
    auto lengthScore =
        similarityScore(_a->get_length(), _b->get_length()) * 0.2;
    auto score = nameScore + artistScore + lengthScore;

    std::cout << _a->get_id() << " == " << _b->get_id() << ": " << score
              << "\n\tname: " << _a->get_name() << " == " << _b->get_name()
              << ": " << nameScore << "\n\tartist: " << _a->get_name()
              << " == " << _b->get_artist() << ": " << artistScore
              << "\n\tlength: " << _a->get_length()
              << " == " << _b->get_length() << ": " << lengthScore << std::endl;
    return score;
}

// Convert string to lowercase
std::string TrackIdentificationService::toLower(const std::string &_str) {
    std::string result = _str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Compute Levenshtein distance
int TrackIdentificationService::levenshteinDistance(const std::string &_s1,
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
double TrackIdentificationService::similarityScore(const std::string &_s1,
                                                   const std::string &_s2) {
    int dist = levenshteinDistance(toLower(_s1), toLower(_s2));
    int maxLength = std::max(_s1.size(), _s2.size());
    return 1.0 - (double)dist / maxLength; // Normalize to 0-1
}

// Function to compute similarity score between two durations
double TrackIdentificationService::similarityScore(int duration1,
                                                   int duration2) {
    int diff = std::abs(duration1 - duration2);

    // If exact match, return 1.0
    if (diff == 0)
        return 1.0;

    // Exponential decay to reduce score as difference increases
    double decayFactor =
        0.05; // Controls how fast the score drops (adjust if needed)
    return std::exp(-decayFactor * diff);
}

double
TrackIdentificationService::similarityScoreDate(const std::string &_date1,
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

bool TrackIdentificationService::findInString(const std::string &_base,
                                              const std::string &_searchText) {
    return toLower(_base).find(toLower(_searchText)) != std::string::npos;
}

std::vector<std::string>
TrackIdentificationService::splitArtists(std::string _artist) {
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

double
TrackIdentificationService::scoreArtistMatch(const std::string &_title,
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
