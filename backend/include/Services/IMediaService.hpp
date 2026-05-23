#pragma once

#include <iostream>
#include <nlohmann/json.hpp>

class IMediaService {
  public:
    virtual ~IMediaService() = default;

    enum class MediaSourceId { Discogs, Local, Spotify, YouTube };

    virtual constexpr MediaSourceId get_id() const = 0;
    virtual bool load(std::shared_ptr<class IMediaEntity> _obj) = 0;

    template <typename T>
    static T getOptional(const nlohmann::json &j, const char *key, T fallback,
                         bool &_fallbackUsed) {

        auto it = j.find(key);

        if (it == j.end() || it->is_null()) {
            _fallbackUsed = true;
            std::cout << "[DEBUG]: Using fallback '" << fallback
                      << "' for value '" << key << "'" << std::endl;
            return fallback;
        }

        return it->get<T>();
    }
};