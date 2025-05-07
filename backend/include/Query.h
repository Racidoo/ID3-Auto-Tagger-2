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
  private:
    std::string accessToken;
    std::string clientId;
    std::string clientSecret;
    std::chrono::steady_clock::time_point tokenExpirationTime;

    inline const static std::filesystem::path pathCredentials =
        std::filesystem::current_path() / "backend" / "api" /
        "credentials.json";

  protected:
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
    bool initialized;

    // void ensureExists(const std::filesystem::path &path);

  public:
    Query(const std::string &_type);
    Query(const std::string &_type, const std::string &_clientId,
          const std::string &_clientSecret, const std::string &_accessToken);
    virtual ~Query();

    inline std::string get_clientId() const { return clientId; };
    inline std::string get_clientSecret() const { return clientSecret; };
    inline bool is_initialized() const { return initialized; }
    
    void set_accessToken(const std::string &_accessToken);
    void set_clientId(const std::string &_clientId);
    void set_clientSecret(const std::string &_clientSecret);

    std::string getValidToken();
    void saveCredentials();

    // Callback function to capture response data
    size_t static writeCallback(void *contents, size_t size, size_t nmemb,
                                std::string *output);
    size_t static writeImageCallback(void *contents, size_t size, size_t nmemb,
                                     std::vector<char> *output);
};

#endif // QUERY_H
