#include "../include/Query.h"

Query::Query(const std::string &_type)
    : tokenExpirationTime(std::chrono::steady_clock::now()), type(_type) {
    getValidToken();
}

Query::~Query() {}

std::string Query::getValidToken() {
    if (!isTokenValid()) {
        accessToken = generateAccessToken();
    }
    return accessToken; // Use the stored token
}

bool Query::isTokenValid() const {
    // return true, if a token has no expiry time
    return (tokenExpirationTime == std::chrono::steady_clock::time_point(
                                       std::chrono::seconds(0))
                ? true
                : std::chrono::steady_clock::now() < tokenExpirationTime);
}

/**
 * @brief
 *
 * @param _type "Google" | "Spotify"
 * @return const std::string&
 */
std::string Query::generateAccessToken() {

    if (!std::filesystem::exists(pathCredentials)) {
        std::cerr << "Could not open " << pathCredentials
                  << ". Created new file. Please ensure proper content!"
                  << std::endl;
        if (pathCredentials.has_extension()) {
            std::filesystem::create_directories(pathCredentials.parent_path());
            std::ofstream(pathCredentials);
        } else {
            std::filesystem::create_directories(pathCredentials);
        }
        json jsonTempCredentials;
        jsonTempCredentials["OAuth"]["Google"]["auth_token"] = "";
        jsonTempCredentials["OAuth"]["Spotify"]["client_id"] = "";
        jsonTempCredentials["OAuth"]["Spotify"]["client_secret"] = "";
        jsonTempCredentials["OAuth"]["Spotify"]["token_uri"] =
            "https://accounts.spotify.com/api/token";

        std::ofstream(pathCredentials) << jsonTempCredentials.dump(4);
    }

    auto fileCredentials = std::ifstream(pathCredentials);
    if (!fileCredentials.is_open()) {
        std::cerr << "Could not open " << pathCredentials << std::endl;
    }
    json jsonCredentials = json::parse(fileCredentials).at("OAuth");

    if (type == "Google") {
        if (!jsonCredentials.contains("Google") ||
            jsonCredentials["Google"].at("auth_token").empty()) {
            throw std::runtime_error(
                "Please provide 'auth_token' from Google API");
        }
        tokenExpirationTime =
            std::chrono::steady_clock::time_point(std::chrono::seconds(0));
        return jsonCredentials["Google"]["auth_token"].get<std::string>();
    } else if (type == "Spotify") {
        if (!jsonCredentials.contains("Spotify") ||
            jsonCredentials["Spotify"].at("client_id").empty() ||
            jsonCredentials["Spotify"].at("client_secret").empty()) {
            throw std::runtime_error("Please provide 'client_id' and "
                                     "'client_secret' from Spotify API");
        }
        if (jsonCredentials["Spotify"].at("token_uri").empty()) {
            throw std::runtime_error(
                "Please provide 'token_uri' from Spotify API");
        }

        CURL *accessTokenCurl = curl_easy_init();
        if (!accessTokenCurl) {
            std::cerr << "Failed to initialize cURL" << std::endl;
            return "";
        }

        std::string response_data;
        std::string postFields =
            "grant_type=client_credentials&client_id=" +
            jsonCredentials["Spotify"]["client_id"].get<std::string>() +
            "&client_secret=" +
            jsonCredentials["Spotify"]["client_secret"].get<std::string>();

        struct curl_slist *accessTokenHeaders = nullptr;
        accessTokenHeaders = curl_slist_append(
            accessTokenHeaders,
            "Content-Type: application/x-www-form-urlencoded");

        // Set cURL options
        curl_easy_setopt(
            accessTokenCurl, CURLOPT_URL,
            jsonCredentials["Spotify"]["token_uri"].get<std::string>().c_str());
        curl_easy_setopt(accessTokenCurl, CURLOPT_POST, 1L);
        curl_easy_setopt(accessTokenCurl, CURLOPT_POSTFIELDS,
                         postFields.c_str());
        curl_easy_setopt(accessTokenCurl, CURLOPT_HTTPHEADER,
                         accessTokenHeaders);
        curl_easy_setopt(accessTokenCurl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(accessTokenCurl, CURLOPT_WRITEDATA, &response_data);

        CURLcode res = curl_easy_perform(accessTokenCurl);
        curl_easy_cleanup(accessTokenCurl);
        curl_slist_free_all(accessTokenHeaders);

        if (res != CURLE_OK) {
            std::cerr << "cURL request failed: " << curl_easy_strerror(res)
                      << std::endl;
            return "";
        }

        // Parse JSON response
        json response_json = json::parse(response_data);
        if (response_json.contains("access_token")) {
            tokenExpirationTime =
                std::chrono::steady_clock::now() +
                std::chrono::seconds(response_json.at("expires_in").get<int>());
            return response_json["access_token"];
        } else {
            throw std::runtime_error(
                "Failed to retrieve access token! Response:" +
                response_json.dump(4));
        }
    } else {
        std::cout << "Not supported" << std::endl;
        throw std::runtime_error("'" + type + " is not supported");
    }
}

// void Query::ensureExists(const std::filesystem::path& path) {
//     if (std::filesystem::exists(path)) return;
//     if (path.has_extension()) {
//         std::filesystem::create_directories(path.parent_path());
//         std::ofstream(path);
//     } else {
//         std::filesystem::create_directories(path);
//     }
// }

/**
 * @brief curl callback needed for general textual data
 *
 * @param contents
 * @param size
 * @param nmemb
 * @param output
 * @return size_t
 */
size_t Query::writeCallback(void *contents, size_t size, size_t nmemb,
                            std::string *output) {
    size_t total_size = size * nmemb;
    if (output) {
        output->append((char *)contents, total_size);
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
size_t Query::writeImageCallback(void *contents, size_t size, size_t nmemb,
                                 std::vector<char> *output) {
    size_t total_size = size * nmemb;
    output->insert(output->end(), (char *)contents,
                   (char *)contents + total_size);
    return total_size;
}

// Convert string to lowercase
std::string Query::toLower(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Compute Levenshtein distance
int Query::levenshteinDistance(const std::string &s1, const std::string &s2) {
    size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

    for (size_t i = 0; i <= len1; ++i)
        dp[i][0] = i;
    for (size_t j = 0; j <= len2; ++j)
        dp[0][j] = j;

    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({dp[i - 1][j] + 1,          // Deletion
                                 dp[i][j - 1] + 1,          // Insertion
                                 dp[i - 1][j - 1] + cost}); // Substitution
        }
    }
    return dp[len1][len2];
}

// Calculate a similarity score
double Query::similarityScore(const std::string &s1, const std::string &s2) {
    int dist = levenshteinDistance(toLower(s1), toLower(s2));
    int maxLength = std::max(s1.size(), s2.size());
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

double Query::similarityScoreDate(const std::string &date1,
                                  const std::string &date2) {
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
    return similarityScore(daysSinceEpoch(date1), daysSinceEpoch(date2));
}

std::vector<char> Query::downloadImage(const std::string &_url,
                                       const std::string &_outputPath) const {
    std::vector<char> imageData;
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