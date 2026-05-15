#include "../include/Query.h"

void Query::ensureCredentialFile() const {
    std::filesystem::create_directories(pathCredentials.parent_path());

    if (!std::filesystem::exists(pathCredentials)) {
        std::ofstream file(pathCredentials);
        if (!file)
            throw std::runtime_error("Failed to create " +
                                     pathCredentials.string());

        file << "{}"; // valid empty JSON
    }
}

void Query::saveCredentialsToFile(const json &_credentials) const {

    std::ofstream file(pathCredentials);
    if (!file)
        throw std::runtime_error("Failed to write " + pathCredentials.string());

    file << _credentials.dump(4);
}

json Query::loadCredentials() const {
    ensureCredentialFile();

    std::ifstream file(pathCredentials);
    if (!file)
        throw std::runtime_error("Failed to open " + pathCredentials.string());

    json data;
    file >> data;
    return data;
}

json Query::performRequest(const std::string &_url) {

    CURL *curl = curl_easy_init();

    if (!curl) {
        std::cerr << "curl not initialized!" << std::endl;
        return {};
    }

    std::string responseData;
    std::string headerData;

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    struct curl_slist *headers = nullptr;

    prepareHeaders(headers);

    auto url = prepareUrl(_url);

    std::cout << "Request: " << url << std::endl;

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerData);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

#ifdef DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    CURLcode res = curl_easy_perform(curl);

    long responseCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "Request failed\n"
                  << "CURL code: " << res << "\n"
                  << "CURL error: " << errorBuffer << "\n"
                  << "URL: " << url << "\n"
                  << "HTTP status: " << responseCode << "\n"
                  << "Headers:\n"
                  << headerData << "\n"
                  << "Response:\n"
                  << responseData << std::endl;

        return {};
    }

    json response = json::parse(responseData, nullptr, false);

    if (response.is_discarded()) {
        std::cerr << "Failed to parse JSON\n"
                  << "Response:\n"
                  << responseData << std::endl;
        return {};
    }

    if (response.contains("error")) {
        std::cerr << response.dump(4) << std::endl;
    }
    // std::cout << response.dump(4) << std::endl;
    std::ofstream("response.json") << response.dump(4);
    return response;
}

/**
 * @brief curl callback needed for general textual data
 *
 * @param contents
 * @param size
 * @param nmemb
 * @param output
 * @return size_t
 */
size_t Query::writeCallback(void *_contents, size_t _size, size_t _nmemb,
                            std::string *_output) {
    size_t total_size = _size * _nmemb;
    if (_output) {
        _output->append((char *)_contents, total_size);
    }
    return total_size;
}

/**
 * @brief curl callback needed for binary data like images
 *
 * @param contents
 * @param size
 * @param nmemb
 * @param output
 * @return size_t
 */
size_t Query::writeImageCallback(void *_contents, size_t _size, size_t _nmemb,
                                 std::vector<char> *_output) {
    size_t total_size = _size * _nmemb;
    _output->insert(_output->end(), (char *)_contents,
                    (char *)_contents + total_size);
    return total_size;
}

// Convert string to lowercase
std::string Query::toLower(const std::string &_str) {
    std::string result = _str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Compute Levenshtein distance
int Query::levenshteinDistance(const std::string &_s1, const std::string &_s2) {
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
double Query::similarityScore(const std::string &_s1, const std::string &_s2) {
    int dist = levenshteinDistance(toLower(_s1), toLower(_s2));
    int maxLength = std::max(_s1.size(), _s2.size());
    return 1.0 - (double)dist / maxLength; // Normalize to 0-1
}

// Function to compute similarity score between two durations
double Query::similarityScore(int duration1, int duration2) {
    int diff = std::abs(duration1 - duration2);

    // If exact match, return 1.0
    if (diff == 0)
        return 1.0;

    // Exponential decay to reduce score as difference increases
    double decayFactor =
        0.1; // Controls how fast the score drops (adjust if needed)
    return std::exp(-decayFactor * diff);
}

double Query::similarityScoreDate(const std::string &_date1,
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

std::vector<std::byte> Query::downloadImage(const std::string &_url,
                                            const std::string &_outputPath) {
    std::vector<std::byte> imageData;

    CURL *curl = curl_easy_init();

    if (!curl)
        return {};

    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    FILE *fp = nullptr;

    if (!_outputPath.empty()) {
        fp = fopen(_outputPath.c_str(), "wb");

        if (!fp) {
            std::cerr << "Failed to open file for writing\n";
            curl_easy_cleanup(curl);
            return {};
        }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nullptr);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    } else {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeImageCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);
    }

    auto start = std::chrono::high_resolution_clock::now();

    CURLcode res = curl_easy_perform(curl);

    auto end = std::chrono::high_resolution_clock::now();

    if (fp)
        fclose(fp);

    if (res == CURLE_OK) {
        PerformanceStats stats;

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &stats.responseCode);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &stats.totalTime);
        curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME,
                          &stats.nameLookupTime);
        curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &stats.connectTime);
        curl_easy_getinfo(curl, CURLINFO_APPCONNECT_TIME,
                          &stats.appConnectTime);
        curl_easy_getinfo(curl, CURLINFO_PRETRANSFER_TIME,
                          &stats.preTransferTime);
        curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME,
                          &stats.startTransferTime);
        curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &stats.downloadSpeed);
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T,
                          &stats.downloadedBytes);

        printCurlStats(stats, _url);
    } else {
        std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(res)
                  << '\n';
    }

    curl_easy_cleanup(curl);

    return imageData;
}

std::string Query::urlEncode(const std::string &_value) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return {};
    }
    char *escaped = curl_easy_escape(curl, _value.c_str(), _value.length());
    std::string result = escaped ? escaped : "";
    curl_free(escaped);
    curl_easy_cleanup(curl);
    return result;
}

void Query::printCurlStats(const PerformanceStats &_stats,
                           const std::string &_url) {
    std::cout << "\n=== CURL PERFORMANCE ===\n";
    std::cout << "URL: " << _url << '\n';
    std::cout << "HTTP Code: " << _stats.responseCode << '\n';

    std::cout << std::fixed << std::setprecision(3);

    std::cout << "DNS Lookup:      " << _stats.nameLookupTime << " s\n";
    std::cout << "TCP Connect:     " << _stats.connectTime << " s\n";
    std::cout << "TLS Handshake:   " << _stats.appConnectTime << " s\n";
    std::cout << "Pre-transfer:    " << _stats.preTransferTime << " s\n";
    std::cout << "TTFB:            " << _stats.startTransferTime << " s\n";
    std::cout << "Total Time:      " << _stats.totalTime << " s\n";

    std::cout << "Downloaded Size: " << _stats.downloadedBytes / 1024.0
              << " KB\n";

    std::cout << "Average Speed:   " << _stats.downloadSpeed / 1024.0
              << " KB/s\n";

    std::cout << "========================\n";
}

json Query::exec(const std::string &_cmd) const {
    // Needs reworking
    std::array<char, 128> buffer;
    std::string result = "[";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(_cmd.c_str(), "r"),
                                                  pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    bool first = true;
    std::string line;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        if (!first && (*(result.end() - 1) == '\n')) {
            result += ",";
        }
        result += buffer.data();
        first = false;
    }
    return json::parse(result += "]");
}
