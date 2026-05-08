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

std::string Query::prepareUrl(const std::string &_url) { return _url; }

json Query::performRequest(const std::string &_url) {

    CURL *curl = curl_easy_init();

    if (!curl) {
        std::cerr << "curl not initialized!" << std::endl;
        return {};
    }

    std::string responseData;

    struct curl_slist *headers = nullptr;

    prepareHeaders(headers);
    auto url = prepareUrl(_url);

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "Request failed: " << res
                  << "\nRequest: " << prepareUrl(_url)
                  << "\nResponse: " << responseData << std::endl;
        return {};
    }

    json response = json::parse(responseData, nullptr, false);
    if (response.is_discarded()) {
        std::cerr << "Error: Failed to parse JSON response!" << std::endl;
        return {};
    }
    if (response.contains("error")) {
        std::cerr << response.dump(4) << std::endl;
    }
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
    auto curl(curl_easy_init());
    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    if (!_outputPath.empty()) {
        FILE *fp = fopen(_outputPath.c_str(), "wb");
        if (!fp) {
            std::cerr << "Failed to open file for writing\n";
            return {};
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    } else {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeImageCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);
    }

    CURLcode res = curl_easy_perform(curl);
    if (curl)
        curl_easy_cleanup(curl);
    return imageData;
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
