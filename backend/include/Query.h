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
    // Query(const std::string &_type);
    // Query(const std::string &_type, const std::string &_clientId,
    //       const std::string &_clientSecret, const std::string &_accessToken);
    virtual ~Query() = default;

    // inline std::string get_clientId() const { return clientId; };
    // inline std::string get_clientSecret() const { return clientSecret; };
    // inline bool is_initialized() const { return initialized; }

    void set_accessToken(const std::string &_accessToken);
    void set_clientId(const std::string &_clientId);
    void set_clientSecret(const std::string &_clientSecret);

    // std::string getValidToken();
    virtual void saveCredentials() = 0;
    virtual std::string generateAccessToken() = 0;
    // void saveCredentials();
    // json loadCredentials() const;

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

    // json headerRequest(const std::string &_request);

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
    // bool initialized;

    void ensureCredentialFile() const;
};

#endif // QUERY_H
