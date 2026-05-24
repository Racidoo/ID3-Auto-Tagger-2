#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Interfaces/ISearchResult.hpp"

class ITrack;
class IVideo;

class TrackIdentificationService {
  public:
    struct SearchQuery {
        std::string title;
        std::string artist;
        std::string album;
        std::string filename;
        std::size_t durationSeconds = 0;

        SearchQuery(std::shared_ptr<ITrack> _track);
    };

    struct AggregatedTrack {
        std::map<double, std::shared_ptr<ITrack>> sources;
        std::map<double, std::shared_ptr<IVideo>> videos;

        // std::string title;
        // std::string album;
        // std::vector<std::string> artists;

        // int duration = 0;

        std::shared_ptr<ITrack> primary;
    };

    static std::shared_ptr<ITrack>
    findBestMatch(const SearchQuery &_query,
                  const std::vector<std::shared_ptr<ITrack>> &_candidates);

    static std::vector<AggregatedTrack>
    aggregate(const std::vector<ISearchResult> &_results);

    static AggregatedTrack merge(std::shared_ptr<ITrack> _track,
                                 const std::vector<ISearchResult> &_results);

  private:
    static bool isSameTrack(const std::shared_ptr<ITrack> &_a,
                            const std::shared_ptr<ITrack> &_b);
    static bool isSameTrack(const std::shared_ptr<IVideo> &_a,
                            const std::shared_ptr<ITrack> &_b);

    static double scoreTracks(const std::shared_ptr<ITrack> &_a,
                              const std::shared_ptr<ITrack> &_b);
    static double scoreTracks(const std::shared_ptr<IVideo> &_a,
                              const std::shared_ptr<ITrack> &_b);

    static std::string toLower(const std::string &_str);
    static double similarityScore(const std::string &_s1,
                                  const std::string &_s2);
    static double similarityScore(int _duration1, int _duration2);
    static double similarityScoreDate(const std::string &_date1,
                                      const std::string &_date2);
    static int levenshteinDistance(const std::string &_s1,
                                   const std::string &_s2);
    static bool findInString(const std::string &_base,
                             const std::string &_searchText);

    static std::vector<std::string> splitArtists(std::string _artist);

    static double scoreArtistMatch(const std::string &_title,
                                   const std::string &_artist);

    static inline std::vector<std::string> collaborationSeparators = {
        ",",     "&",           " x ",    " X ", " feat. ",
        " ft. ", " featuring ", " with ", "+"};
};