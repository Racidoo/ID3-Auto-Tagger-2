#pragma once

#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

using json = nlohmann::json;

class QueryObject {
  public:
    enum class MetadataState { Preview, Partial, Full };

    QueryObject(const std::string &_id, const std::string &_name,
                MetadataState _state, const std::string &_imageURL);
    ~QueryObject();

    const std::string &get_id() const;
    const std::string &get_name() const;
    MetadataState get_state() const;
    const std::string &get_imageUrl() const;
    std::vector<std::byte> get_image() const;

    void set_state(MetadataState _state);

    template <typename T>
    static std::string vecToStr(const std::vector<T> &_object,
                                const std::string &_sep = ", ") {
        static_assert(std::is_base_of<QueryObject, T>::value,
                      "T must derive from QueryObject");

        std::stringstream ss;
        for (size_t i = 0; i < _object.size(); ++i) {
            if (i != 0)
                ss << _sep;
            ss << _object[i].get_name();
        }
        return ss.str();
    }

    template <typename T>
    static T getOptional(const json &j, const char *key, T fallback,
                         bool &_fallbackUsed) {

        auto it = j.find(key);

        if (it == j.end() || it->is_null()) {
            _fallbackUsed = true;
            return fallback;
        }

        return it->get<T>();
    }

  protected:
    std::string id;
    std::string name;
    MetadataState state;
    std::string imageURL;
};
