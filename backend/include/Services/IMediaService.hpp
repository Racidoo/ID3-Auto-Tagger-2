#pragma once

#include <nlohmann/json.hpp>

class IMediaService {
  public:
    virtual ~IMediaService() = default;

    virtual void load(std::shared_ptr<class IMediaEntity> _obj) = 0;

    template <typename T>
    static T getOptional(const nlohmann::json &j, const char *key, T fallback,
                         bool &_fallbackUsed) {

        auto it = j.find(key);

        if (it == j.end() || it->is_null()) {
            _fallbackUsed = true;
            return fallback;
        }

        return it->get<T>();
    }
};