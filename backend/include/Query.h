#if !defined(QUERY_H)
#define QUERY_H

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

    std::string urlEncode(const std::string &_value);

  protected:
    struct PerformanceStats {
        long responseCode = 0;

        double totalTime = 0.0;
        double nameLookupTime = 0.0;
        double connectTime = 0.0;
        double appConnectTime = 0.0;
        double preTransferTime = 0.0;
        double startTransferTime = 0.0;

        double downloadSpeed = 0.0;

        curl_off_t downloadedBytes = 0;
    };

    inline const static std::filesystem::path pathCredentials =
        std::filesystem::current_path() / "backend" / "api" /
        "credentials.json";
    std::chrono::steady_clock::time_point tokenExpirationTime;

    json loadCredentials() const;
    void saveCredentialsToFile(const json &_credentials) const;

    virtual void prepareHeaders(struct curl_slist *&_headers) = 0;
    virtual std::string prepareUrl(const std::string &_url) = 0;
    json performRequest(const std::string &_url);

    static void printCurlStats(const PerformanceStats &_stats,
                               const std::string &_url);

    json exec(const std::string &_cmd) const;

  private:
    void ensureCredentialFile() const;
};

#endif // QUERY_H
