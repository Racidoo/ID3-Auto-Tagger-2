#if !defined(QUERY_H)
#define QUERY_H

#include "../lib/base64/base64.h"

#include <chrono>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Query {
  protected:
    std::string accessToken;
    std::chrono::steady_clock::time_point tokenExpirationTime;

    inline const static std::filesystem::path pathCredentials =
        std::filesystem::current_path() / "backend" / "api" /
        "credentials.json";

    bool isTokenValid() const;
    std::string generateAccessToken();
    static std::string toLower(const std::string &str);
    static double similarityScore(const std::string &s1, const std::string &s2);
    static double similarityScore(int duration1, int duration2);
    static double similarityScoreDate(const std::string &date1,
                                      const std::string &date2);
    static int levenshteinDistance(const std::string &s1,
                                   const std::string &s2);

    json exec(const std::string &_cmd) const;

  public:
    std::vector<char> downloadImage(const std::string &_url,
                                    const std::string &_outputPath = "") const;

  private:
    const std::string type;

  public:
    Query(const std::string &_type);
    virtual ~Query();

    std::string getValidToken();

    // Callback function to capture response data
    size_t static writeCallback(void *contents, size_t size, size_t nmemb,
                                std::string *output);
    size_t static writeImageCallback(void *contents, size_t size, size_t nmemb,
                                     std::vector<char> *output);
};

#endif // QUERY_H
