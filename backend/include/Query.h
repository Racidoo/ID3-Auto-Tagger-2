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
  public:
    virtual ~Query() = default;

    virtual void saveCredentials() = 0;
    virtual std::string generateAccessToken() = 0;

    // Callback function to capture response data
    size_t static writeCallback(void *_contents, size_t _size, size_t _nmemb,
                                std::string *output);
    size_t static writeImageCallback(void *_contents, size_t _size,
                                     size_t _nmemb, std::vector<char> *_output);
    static std::vector<std::byte>
    downloadImage(const std::string &_url, const std::string &_outputPath = "");

  protected:
    inline const static std::filesystem::path pathCredentials =
        std::filesystem::current_path() / "backend" / "api" /
        "credentials.json";
    std::chrono::steady_clock::time_point tokenExpirationTime;

    json loadCredentials() const;
    void saveCredentialsToFile(const json &_credentials) const;

    virtual void prepareHeaders(struct curl_slist *&_headers) = 0;
    virtual std::string prepareUrl(const std::string &_url);
    json performRequest(const std::string &_url);

    static std::string toLower(const std::string &_str);
    static double similarityScore(const std::string &_s1,
                                  const std::string &_s2);
    static double similarityScore(int _duration1, int _duration2);
    static double similarityScoreDate(const std::string &_date1,
                                      const std::string &_date2);
    static int levenshteinDistance(const std::string &_s1,
                                   const std::string &_s2);

    json exec(const std::string &_cmd) const;

  private:
    void ensureCredentialFile() const;
};

#endif // QUERY_H
